
#include <fstream>

#include "db_frame_manager.hpp"
#include "db_log.hpp"
#include "utilities.h"

DeepBreathFrameManager* DeepBreathFrameManager::_frame_manager = nullptr;

DeepBreathFrameManager & DeepBreathFrameManager::getInstance()
{
	if (_frame_manager == nullptr) {
		_frame_manager = new DeepBreathFrameManager();
	}

	return *_frame_manager;
}

DeepBreathFrameManager::DeepBreathFrameManager(unsigned int n_frames, const char * frame_disk_path) :
	_n_frames(n_frames), _frame_disk_path(frame_disk_path), _oldest_frame_index(0), interval_active(false)
{
	manager_start_time = clock();
	_frame_data_arr = new DeepBreathFrameData*[_n_frames];
	for (unsigned int i = 0; i < _n_frames; i++) {
		_frame_data_arr[i] = nullptr;
	}
}

DeepBreathFrameManager::~DeepBreathFrameManager()
{
	cleanup();
	if (_frame_data_arr != nullptr) {
		delete[](_frame_data_arr);
	}
}


void DeepBreathFrameManager::reset() {
	frame_idx = 1;
	first_timestamp = NULL;
	_oldest_frame_index = 0;
	interval_active = false;
	manager_start_time = clock();
	for (unsigned int i = 0; i < _n_frames; i++) {
		_frame_data_arr[i] = NULL;
	}
}

int DeepBreathFrameManager::get_frames_array_size() {
	int c = 0;
	for (unsigned int i = 0; i < _n_frames; i++) {
		if (_frame_data_arr[i] != NULL) {
			c++;
		}
	}
	return c;
}

void DeepBreathFrameManager::process_frame(const rs2::video_frame& color_frame, const rs2::depth_frame& depth_frame)
{
	DeepBreathLog& log = DeepBreathLog::getInstance();
	assert(log); //log instance must be initiated before frame processing (i.e. "start camera" or "load file" before cv notify)
	DeepBreathFrameData * breathing_data = new DeepBreathFrameData();

	identify_markers(color_frame, depth_frame, breathing_data);

	//if user config dimension is 3D, check for 0,-0,0 3d coordinates. dump such frames
	if (DeepBreathConfig::getInstance().dimension == dimension::D3) {
		if (check_illegal_3D_coordinates(breathing_data)) {
			frames_dumped_in_row++;
			log.log_file << "Warning: illegal 3D coordinates! frame was dumped.,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,"; //NOTE: NUMBER OF ',' CHARACTERS MUST REMAIN AS IS!
			if (frames_dumped_in_row >= 3) {								//This is required for transition to next columns in the log file! 
				log.log_file << '\n';
				cleanup();
			}
			return;
		}
		else {
			frames_dumped_in_row = 0;
		}
	}
	//calculate 2D distances:
	breathing_data->CalculateDistances2D();

	//calculate 3D distances:
	breathing_data->CalculateDistances3D();

	//update timestamps of the current frame:
	update_timestamps(color_frame, depth_frame, breathing_data);

	//check if frame id duplicated
	bool is_dup = false;
	int last_frame_index = (_n_frames + _oldest_frame_index - 1) % _n_frames;
	if (_frame_data_arr[last_frame_index] != NULL) {
		if (DeepBreathConfig::getInstance().dimension == dimension::D2) {
			if (_frame_data_arr[last_frame_index]->color_timestamp == breathing_data->color_timestamp
				&& _frame_data_arr[last_frame_index]->average_2d_dist == breathing_data->average_2d_dist) {
				is_dup = true;
			}
		}
		if (DeepBreathConfig::getInstance().dimension == dimension::D3) {
			if (_frame_data_arr[last_frame_index]->color_timestamp == breathing_data->color_timestamp
				&& _frame_data_arr[last_frame_index]->depth_timestamp == breathing_data->depth_timestamp
				&& _frame_data_arr[last_frame_index]->average_3d_dist == breathing_data->average_3d_dist) {
				is_dup = true;
			}
		}
	}
	if (!is_dup) {
		//for logging
		breathing_data->GetDescription();
		add_frame_data(breathing_data);
	}
	else {
		log.log_file << ",,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,";
		//NOTE: NUMBER OF ',' CHARACTERS MUST REMAIN AS IS! This is required for transition to next columns in the log file!
	}
}

void DeepBreathFrameManager::identify_markers(const rs2::video_frame& color_frame, const rs2::depth_frame& depth_frame, DeepBreathFrameData* breathing_data)
{

	DeepBreathConfig& user_cfg = DeepBreathConfig::getInstance();
	if (user_cfg.is_stickers) { //use stickers and identify them

			// Create bgr mode matrix of color_frame
		const void * color_frame_data = color_frame.get_data();
		cv::Mat rgb8_mat(cv::Size(color_frame.get_width(), color_frame.get_height()), CV_8UC3, (void *)color_frame_data, cv::Mat::AUTO_STEP);
		cv::Mat hsv8_mat(cv::Size(color_frame.get_width(), color_frame.get_height()), CV_8UC3);
		cv::cvtColor(rgb8_mat, hsv8_mat, cv::COLOR_RGB2HSV);

		cv::Scalar color_range_low;
		cv::Scalar color_range_high;
		int low_thresh = 100;
		int high_thresh = 255;

		//find required color:
		switch (DeepBreathConfig::getInstance().color) {
		case(YELLOW):
			//RECOMMENDED
			//hsv opencv official yellow range: (20, 100, 100) to (30, 255, 255)
			color_range_low = cv::Scalar(20, 50, 50);
			color_range_high = cv::Scalar(40, 255, 255);
			break;
		case(BLUE):
			color_range_low = cv::Scalar(90, 20, 10);
			color_range_high = cv::Scalar(135, 255, 255);
			low_thresh = 10;
			break;
		case(GREEN):
			color_range_low = cv::Scalar(35, 30, 30);
			color_range_high = cv::Scalar(85, 255, 255);
			low_thresh = 20;
			break;
		case(RED): //Red implementation missing: there are two foreign ranges of red, requires a slight change in implementation
		//	color_range_low = cv::Scalar();
		//	color_range_high = cv::Scalar();
			break;
		}

		cv::Mat color_only_mask(cv::Size(color_frame.get_width(), color_frame.get_height()), CV_8UC1);
		inRange(hsv8_mat, color_range_low, color_range_high, color_only_mask);
		cv::Mat color_only_mat(cv::Size(color_frame.get_width(), color_frame.get_height()), CV_8UC3, cv::Scalar(0, 0, 0));
		hsv8_mat.copyTo(color_only_mat, color_only_mask);

		//Pick up the grayscale
		cv::Mat color_only_bgr8_mat(cv::Size(color_frame.get_width(), color_frame.get_height()), CV_8UC3);
		cvtColor(color_only_mat, color_only_bgr8_mat, cv::COLOR_HSV2BGR);
		cv::Mat color_only_grayscale_mat(cv::Size(color_frame.get_width(), color_frame.get_height()), CV_8UC3);
		cvtColor(color_only_bgr8_mat, color_only_grayscale_mat, cv::COLOR_BGR2GRAY);

		//create binary image:
		cv::Mat image_th;
		cv::Mat bin_mat(color_only_grayscale_mat.size(), color_only_grayscale_mat.type());

		// simple threshold worked better than adaptive
		cv::threshold(color_only_grayscale_mat, image_th, low_thresh, high_thresh, cv::THRESH_BINARY);

		//connected components:
		cv::Mat1i labels;
		cv::Mat1i stats;
		cv::Mat1d centroids;
		cv::connectedComponentsWithStats(image_th, labels, stats, centroids);

		//calc threshold for connected component area (max area/2)
		int area_threshold = 0;
		for (int i = 1; i < stats.rows; i++) //label 0 is the background
		{
			if (stats[i][cv::CC_STAT_AREA] > area_threshold) {
				area_threshold = stats[i][cv::CC_STAT_AREA];
			}
		}
		area_threshold = area_threshold / 2;

		//get centers:
		for (int i = 1; i < centroids.rows; i++) //label 0 is the background
		{
			if (stats[i][cv::CC_STAT_AREA] > area_threshold) {
				breathing_data->circles.push_back(cv::Vec3f(centroids(i, 0), centroids(i, 1)));
				circle(rgb8_mat, cv::Point(centroids(i, 0), centroids(i, 1)), 3, cv::Scalar(0, 255, 0));
			}
		}

		//distinguish between stickers:
		if (breathing_data->circles.size() < DeepBreathConfig::getInstance().num_of_stickers) {//not all circles were found
			cleanup();
			interval_active = false; //missed frame so calculation has to start all over.
			return;
		}
		breathing_data->UpdateStickersLoactions();

	}
	else { //use AI to identify nipples and bellybutton

	}


}

void DeepBreathFrameManager::update_timestamps(const rs2::video_frame& color_frame, const rs2::depth_frame& depth_frame, DeepBreathFrameData* breathing_data)
{
	//add color timestamp:
	breathing_data->color_timestamp = color_frame.get_timestamp();

	//add depth timestamp:
	breathing_data->depth_timestamp = depth_frame.get_timestamp();

	//add system timestamp:
	clock_t current_system_time = clock();
	breathing_data->system_timestamp = (current_system_time - manager_start_time) / double(CLOCKS_PER_SEC); //time in seconds elapsed since frame manager created

	if (!first_timestamp) {
		first_timestamp =
			(breathing_data->color_timestamp < breathing_data->depth_timestamp) ?
			breathing_data->color_timestamp :
			breathing_data->depth_timestamp;
	}

	breathing_data->frame_idx = frame_idx;
	frame_idx++;
	breathing_data->color_idx = color_frame.get_frame_number();
	breathing_data->depth_idx = depth_frame.get_frame_number();
	breathing_data->system_color_timestamp = (breathing_data->color_timestamp - first_timestamp) / double(CLOCKS_PER_SEC); //time elapsed from first timestamp in video - which timestamp?
	breathing_data->system_depth_timestamp = (breathing_data->depth_timestamp - first_timestamp) / double(CLOCKS_PER_SEC);

}

void DeepBreathFrameManager::cleanup()
{
	if (_frame_data_arr != nullptr) {

		for (unsigned int i = 0; i < _n_frames; i++) {
			if (_frame_data_arr[i] != nullptr) {
				free(_frame_data_arr[i]);
			}
			_frame_data_arr[i] = nullptr;
		}
	}

	DeepBreathLog& log = DeepBreathLog::getInstance();
	assert(log); //log instance must be initiated before frame processing (i.e. "start camera" or "load file" before cv notify)
	log.log_file << "frames array cleanup...\n,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,";
	//NOTE: NUMBER OF ',' CHARACTERS MUST REMAIN AS IS! This is required for transition to next columns in the log file!
	frames_dumped_in_row = 0;
}

void DeepBreathFrameManager::add_frame_data(DeepBreathFrameData * frame_data)
{
	// delete last frame
	if (_frame_data_arr[_oldest_frame_index] != NULL) {
		free(_frame_data_arr[_oldest_frame_index]);
		_frame_data_arr[_oldest_frame_index] = NULL;
	}

	_frame_data_arr[_oldest_frame_index] = frame_data;
	_oldest_frame_index = (_oldest_frame_index + 1) % _n_frames;
}


void DeepBreathFrameManager::get_locations(stickers s, std::vector<cv::Point2d> *out) {
	if (DeepBreathConfig::getInstance().mode != graph_mode::LOCATION) {
		DeepBreathLog& log = DeepBreathLog::getInstance();
		assert(log); //log instance must be initiated before frame processing (i.e. "start camera" or "load file" before cv notify)
		log.log_file << "Warning: get_locations was called in incompatible mode! (use L mode),,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,";
		//NOTE: NUMBER OF ',' CHARACTERS MUST REMAIN AS IS! This is required for transition to next columns in the log file!
		return;
	}
	if (_frame_data_arr == NULL) return;

	double current_time = (clock() - manager_start_time) / double(CLOCKS_PER_SEC);
	for (unsigned int i = 0; i < _n_frames; i++) {
		int idx = (_oldest_frame_index + i + _n_frames) % _n_frames; //this is right order GIVEN that get_locations is run after add_frame_data (after _oldest_frame_idx++)
		if (_frame_data_arr[idx] != NULL) {
			//check if frame was received in under 15 sec
			if ((current_time - _frame_data_arr[idx]->system_timestamp) <= 15.0) { //TODO: decide if needed
				out->push_back(cv::Point2d(_frame_data_arr[idx]->system_timestamp, (*_frame_data_arr[idx]->stickers_map_cm[s])[2]));
			}
		}
	}
}

void DeepBreathFrameManager::get_dists(std::vector<cv::Point2d>* out) {
	if (DeepBreathConfig::getInstance().mode == graph_mode::LOCATION) {
		DeepBreathLog& log = DeepBreathLog::getInstance();
		assert(log); //log instance must be initiated before frame processing (i.e. "start camera" or "load file" before cv notify)
		log.log_file << "Warning: get_dists was called in LOCATION mode!,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,";
		//NOTE: NUMBER OF ',' CHARACTERS MUST REMAIN AS IS! This is required for transition to next columns in the log file!
		return;
	}
	if (_frame_data_arr == NULL) return;

	double current_time = (clock() - manager_start_time) / double(CLOCKS_PER_SEC);
	for (unsigned int i = 0; i < _n_frames; i++) {
		int idx = (_oldest_frame_index + i + _n_frames) % _n_frames; //this is right order GIVEN that get_dists is run after add_frame_data (after _oldest_frame_idx++)
		if (_frame_data_arr[idx] != NULL) {
			double avg_dist = (DeepBreathConfig::getInstance().dimension == dimension::D2) ?
				_frame_data_arr[idx]->average_2d_dist :
				_frame_data_arr[idx]->average_3d_dist;
			double t = (DeepBreathConfig::getInstance().dimension == dimension::D2) ?
				_frame_data_arr[idx]->system_color_timestamp :
				_frame_data_arr[idx]->system_depth_timestamp;
			out->push_back(cv::Point2d(t, avg_dist));
		}
	}
}

long double DeepBreathFrameManager::no_graph() {
	std::vector<cv::Point2d> points;
	get_dists(&points);

	long double f;
	if (GET_FREQUENCY_BY_FFT) {
		normalize_distances(&points);
		f = (calc_frequency_fft(&points));
	}
	else {	// get_frequency_differently
		bool cm_units = (DeepBreathConfig::getInstance().dimension == dimension::D3) ?
			true : DeepBreathConfig::getInstance().calc_2d_by_cm;
		f = calc_frequency_differently(&points, cm_units);
	}
	return f;
}

void DeepBreathFrameManager::activateInterval() {
	this->interval_active = true;
}

void DeepBreathFrameManager::deactivateInterval() {
	this->interval_active = false;
}
