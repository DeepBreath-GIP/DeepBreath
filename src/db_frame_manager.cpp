
#include <fstream>

#include "db_frame_manager.hpp"
#include "db_log.hpp"
#include "db_graph_plot.hpp"
//#include <librealsense2/rsutil.h>

//#include "utilities.h"

/* Forward Decalrations: */
static void normalize_distances(std::vector<cv::Point2d>* samples);
static long double calc_and_log_frequency_fft(std::vector<cv::Point2d>* samples, std::vector<cv::Point2d>* out_frequencies = NULL);
static void FFT(short int dir, long m, double *x, double *y);
static bool check_illegal_3D_coordinates(const DeepBreathFrameData* breathing_data);
//static void get_3d_coordinates(const rs2::depth_frame& depth_frame, float x, float y, cv::Vec3f& output);
/* ** */

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
	DeepBreathConfig& user_cfg = DeepBreathConfig::getInstance();

	identify_markers(color_frame, depth_frame, breathing_data);

	//if user config dimension is 3D, check for 0,-0,0 3d coordinates. dump such frames
	if (user_cfg.dimension == dimension::D3) {

		//get 3d coords:
		if (user_cfg.stickers_included[stickers::left]) {
			get_3d_coordinates(depth_frame, (*breathing_data->left)[0], (*breathing_data->left)[1], breathing_data->left_cm);
		}
		if (user_cfg.stickers_included[stickers::right]) {
			get_3d_coordinates(depth_frame, (*breathing_data->right)[0], (*breathing_data->right)[1], breathing_data->right_cm);
		}
		if (user_cfg.stickers_included[stickers::mid1]) {
			get_3d_coordinates(depth_frame, (*breathing_data->mid1)[0], (*breathing_data->mid1)[1], breathing_data->mid1_cm);
		}
		if (user_cfg.stickers_included[stickers::mid2]) {
			get_3d_coordinates(depth_frame, (*breathing_data->mid2)[0], (*breathing_data->mid2)[1], breathing_data->mid2_cm);
		}
		if (user_cfg.stickers_included[stickers::mid3]) {
			get_3d_coordinates(depth_frame, (*breathing_data->mid3)[0], (*breathing_data->mid3)[1], breathing_data->mid3_cm);
		}

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

	if (user_cfg.dimension == D3) {
		//calculate Volumes if in 3D mode:
		breathing_data->CalculateVolumes(depth_frame);
	}

	//update timestamps of the current frame:
	update_timestamps(color_frame, depth_frame, breathing_data);

	//check if frame id duplicated
	bool is_dup = false;
	int last_frame_index = (_n_frames + _oldest_frame_index - 1) % _n_frames;
	if (_frame_data_arr[last_frame_index] != NULL) {
		if (user_cfg.dimension == dimension::D2) {
			if (_frame_data_arr[last_frame_index]->color_timestamp == breathing_data->color_timestamp
				&& _frame_data_arr[last_frame_index]->average_2d_dist == breathing_data->average_2d_dist) {
				is_dup = true;
			}
		}
		if (user_cfg.dimension == dimension::D3) {
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

		//update graph:
		add_data_to_graph(breathing_data);

	}
	else {
		log.log_file << ",,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,";
		//NOTE: NUMBER OF ',' CHARACTERS MUST REMAIN AS IS! This is required for transition to next columns in the log file!
	}
}

long double DeepBreathFrameManager::calculate_breath_rate() {

	DeepBreathConfig& user_cfg = DeepBreathConfig::getInstance();

	long double bpm = 0;

	std::vector<cv::Point2d> points;
	std::vector<cv::Point2d> loc_points[5];

	switch (user_cfg.mode) {
	case VOLUME:
		get_volumes(&points);
		bpm = _calc_bpm_and_log_dists(points);
		break;
	default: //distances, fourier and no graph - all based on distances:
		get_dists(&points);
		bpm = _calc_bpm_and_log_dists(points);
		break;
	}

	return bpm;
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

void DeepBreathFrameManager::add_data_to_graph(DeepBreathFrameData * frame_data) {

	DeepBreathConfig& user_cfg = DeepBreathConfig::getInstance();
	DeepBreathGraphPlot& graph_plot = DeepBreathGraphPlot::getInstance();
	assert(graph_plot); //graph plot instance must be initiated before frame processing (i.e. "start camera" or "load file" before cv notify)
	DeepBreathFrameManager& frame_manager = DeepBreathFrameManager::getInstance();

	cv::Point2d p(0, 0);
	std::vector<cv::Point2d> points;
	std::vector<cv::Point2d> frequency_points;
	long double f;

	switch (user_cfg.mode) {
	case DISTANCES:
		if (user_cfg.dimension == D2) {
			p.x = frame_data->system_color_timestamp;
			p.y = frame_data->average_2d_dist;
		}
		else {
			p.x = frame_data->system_depth_timestamp;
			p.y = frame_data->average_3d_dist;
		}
		graph_plot.addData(p);
		break;
	case FOURIER:
		//DO NOTHING! cannot calculate fft based on single point.
		//Implementation of graph data setting is in: _calc_bpm_and_log_dists for this case. 
		break;
	case LOCATION:
		for (int stInt = stickers::left; stInt != stickers::sdummy; stInt++) {
			stickers s = static_cast<stickers>(stInt);
			if (user_cfg.stickers_included[s]) {
				p.x = frame_data->system_timestamp;
				p.y = (*frame_data->stickers_map_cm[s])[2];
				graph_plot.addData(p, s);
			}
		}
		break;
	case VOLUME:
		p.x = frame_data->system_depth_timestamp;
		if (user_cfg.volume_type == TETRAHEDRON) {
			p.y = frame_data->tetra_volume;
		}
		else {
			p.y = frame_data->reimann_volume;
		}
		graph_plot.addData(p); //graph plot is responsible for the calculation of the difference (see addData)
		break;
	case NOGRAPH:
		//Do nothing, nothing to plot.
		break;
	}

	graph_plot.update();

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

void DeepBreathFrameManager::get_volumes(std::vector<cv::Point2d>* out) {

	DeepBreathConfig& user_cfg = DeepBreathConfig::getInstance();

	if (user_cfg.mode == LOCATION) {
		DeepBreathLog& log = DeepBreathLog::getInstance();
		assert(log); //log instance must be initiated before frame processing (i.e. "start camera" or "load file" before cv notify)
		//TODO: Log bpm info?
		log.log_file << "Warning: get_volumes was called in LOCATION mode!,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,";
		//NOTE: NUMBER OF ',' CHARACTERS MUST REMAIN AS IS! This is required for transition to next columns in the log file!
		return;
	}
	if (user_cfg.dimension != D3) {
		DeepBreathLog& log = DeepBreathLog::getInstance();
		assert(log); //log instance must be initiated before frame processing (i.e. "start camera" or "load file" before cv notify)
		log.log_file << "Warning: get_volumes was called in D2 mode!,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,";
		//NOTE: NUMBER OF ',' CHARACTERS MUST REMAIN AS IS! This is required for transition to next columns in the log file!
		return;
	}

	if (_frame_data_arr == NULL) return;

	double current_time = (clock() - manager_start_time) / double(CLOCKS_PER_SEC);
	for (unsigned int i = 0; i < _n_frames; i++) {
		int idx = (_oldest_frame_index + i + _n_frames) % _n_frames; //this is right order GIVEN that get_volumes is run after add_frame_data (after _oldest_frame_idx++)
		if (_frame_data_arr[idx] != NULL) {
			double d_vol = 0;
			if (user_cfg.volume_type == TETRAHEDRON) {
				d_vol = _frame_data_arr[idx]->tetra_volume;
			}
			else { // REIMANN
				d_vol = _frame_data_arr[idx]->reimann_volume;
			}
			double t = _frame_data_arr[idx]->system_depth_timestamp;
			out->push_back(cv::Point2d(t, d_vol));
		}
	}
}

void DeepBreathFrameManager::get_dists(std::vector<cv::Point2d>* out) {

	if (DeepBreathConfig::getInstance().mode == graph_mode::LOCATION) {
		DeepBreathLog& log = DeepBreathLog::getInstance();
		assert(log); //log instance must be initiated before frame processing (i.e. "start camera" or "load file" before cv notify)
		//TODO: Log bpm info?
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
	normalize_distances(&points);
	f = (calc_and_log_frequency_fft(&points));
	return f;
}

void DeepBreathFrameManager::activateInterval() {
	this->interval_active = true;
}

void DeepBreathFrameManager::deactivateInterval() {
	this->interval_active = false;
}

//long double DeepBreathFrameManager::_calc_bpm_and_log_volumes(std::vector<cv::Point2d>& points) {
//	long double f;
//	//normalize_distances(&points);
//	std::vector<cv::Point2d> frequency_points;
//	f = calc_and_log_frequency_fft(&points, &frequency_points);
//
//	if (points.size() < NUM_OF_LAST_FRAMES * 0.5)
//		f = 0;
//
//	long double bpm = f * 60;
//	return bpm;
//
//}

long double DeepBreathFrameManager::_calc_bpm_and_log_dists(std::vector<cv::Point2d>& points) {
	long double f;
	normalize_distances(&points);
	std::vector<cv::Point2d> frequency_points;
	f = calc_and_log_frequency_fft(&points, &frequency_points);

	if (DeepBreathConfig::getInstance().mode == FOURIER) {

		DeepBreathGraphPlot& graph_plot = DeepBreathGraphPlot::getInstance();
		assert(graph_plot);

		graph_plot.reset();

		for (auto fp : frequency_points) {
			graph_plot.addData(fp);
		}

		graph_plot.update();
	}

	if (points.size() < NUM_OF_LAST_FRAMES * 0.5)
		f = 0;

	long double bpm = f * 60;
	return bpm;

}


/* Normalize ditances to create sample with vivid difference: */
static void normalize_distances(std::vector<cv::Point2d>* samples) {
	if (samples->size() < 3) return;
	double max_dist = samples->at(0).y;
	double min_dist = samples->at(0).y;
	for (int i = 1; i < samples->size(); i++) {
		double curr_dist = samples->at(i).y;
		if (curr_dist > max_dist) max_dist = curr_dist;
		if (curr_dist < min_dist) min_dist = curr_dist;
	}
	for (int i = 0; i < samples->size(); i++) {
		double temp_t = samples->at(i).x;
		double temp_d = samples->at(i).y;
		double norm_d = 2 * (temp_d - min_dist) / (max_dist - min_dist) - 1;
		samples->at(i) = cv::Point2d(temp_t, norm_d);
	}
}

/*
* To be used in D mode
* returns most dominant frequency, calculated for average distance in frames received in the last 15 seconds
* the avg distance is calculated only for distances set to true in user_cfg.dists_included
* Logs the result.
*/
static long double calc_and_log_frequency_fft(std::vector<cv::Point2d>* samples, std::vector<cv::Point2d>* out_frequencies) {

	DeepBreathLog& log = DeepBreathLog::getInstance();
	assert(log);

	if (samples->size() < 5) {
		log.log_file << '\n';
		return 0;
	}
	int realSamplesNum = samples->size();	// N - number of samples (frames)
	int realSamplesLog = log2(realSamplesNum);
	// fft requires that the number of samples is a power of 2. add padding if needed
	const int paddedSamplesNum = (realSamplesNum == pow(2, realSamplesLog)) ? realSamplesNum : pow(2, realSamplesLog + 1);
	int dir = 1;
	long m = log2(paddedSamplesNum);
	double* X = new double[paddedSamplesNum];
	double* Y = new double[paddedSamplesNum];
	// insert real samples in first #realSamplesNum slots
	for (int s = 0; s < realSamplesNum; s++) {
		X[s] = samples->at(s).y;
		Y[s] = 0;	// no imaginary part in samples
	}
	// add padding after real samples
	for (int s = realSamplesNum; s < paddedSamplesNum; s++) {
		X[s] = 0;
		Y[s] = 0;
	}

	double t0 = samples->at(0).x;	// t0, t1 - start, end time(seconds)
	double t1 = samples->at(realSamplesNum - 1).x;
	if (t1 == t0) return 0;
	double fps = realSamplesNum / (t1 - t0);	// FPS - frames per second

	FFT(dir, m, X, Y);

	// following comented code return the average of -top- frequencies found by fourier
	/*const int top = 100;
	int top_max_idx[top] = { 0 };
	double min_bpm = 7.0;
	int mini = ceil((min_bpm / 60.0)*((paddedSamplesNum - 2.0) / fps));	// assume BPM >= min_bpm
	for (int j = 0; j < top; j++) {
		int max_idx = 0;
		double max_val = 0;
		for (int i = mini; i < realSamplesNum / 2; i++) { //frequency 0 always most dominant. ignore first coef.
			double val = abs(X[i])*abs(X[i]) + abs(Y[i])*abs(Y[i]);
			if (val > max_val) {
				max_val = val;
				max_idx = i;
			}
		}
		top_max_idx[j] = max_idx;
		X[max_idx] = 0;
	}
	double avg_max_idx = 0;
	for (int i = 0; i < top; i++)  avg_max_idx += top_max_idx[i];
	avg_max_idx /= top;
	long double f = fps / (paddedSamplesNum - 2.0) * top_max_idx[0];
	long double f_avg = fps / (paddedSamplesNum - 2.0) * avg_max_idx;
	*/

	int max_idx = 0;
	double max_val = 0;
	double min_bpm = 5.0;	// TODO: decide minimal BPM
	int mini = ceil((min_bpm / 60.0)*((paddedSamplesNum - 2.0) / fps));	// assume BPM >= min_bpm
	for (int i = 0; i < realSamplesNum / 2; i++) { //frequency 0 always most dominant. ignore first coef.
		double val = abs(X[i])*abs(X[i]) + abs(Y[i])*abs(Y[i]);
		if (out_frequencies != NULL) {
			double frequency = fps / (paddedSamplesNum - 2.0) * i;
			//out_frequencies->push_back(cv::Point2d(frequency, abs(X[i])));
			out_frequencies->push_back(cv::Point2d(frequency, val));
		}
		if (i >= mini && val > max_val) {
			max_val = val;
			max_idx = i;
		}
	}

	delete X;
	delete Y;

	log.log_file << fps << ',';
	log.log_file << realSamplesNum << ',';
	long double f = fps / (paddedSamplesNum - 2.0) * max_idx;
	log.log_file << std::fixed << std::setprecision(6) << f << ',';
	log.log_file << std::fixed << std::setprecision(6) << f * 60.0 << '\n';
	return f;
}

static void FFT(short int dir, long m, double *x, double *y) {
	long n, i, i1, j, k, i2, l, l1, l2;
	double c1, c2, tx, ty, t1, t2, u1, u2, z;
	/* Calculate the number of points */
	n = 1;
	for (i = 0; i < m; i++)
		n *= 2;
	/* Do the bit reversal */
	i2 = n >> 1;
	j = 0;


	for (i = 0; i < n - 1; i++) {
		if (i < j) {
			tx = x[i];
			ty = y[i];
			x[i] = x[j];
			y[i] = y[j];
			x[j] = tx;
			y[j] = ty;
		}
		k = i2;
		while (k <= j) {
			j -= k;
			k >>= 1;
		}
		j += k;
	}

	/* Compute the FFT */
	c1 = -1.0;
	c2 = 0.0;
	l2 = 1;
	for (l = 0; l < m; l++) {
		l1 = l2;
		l2 <<= 1;
		u1 = 1.0;
		u2 = 0.0;
		for (j = 0; j < l1; j++) {
			for (i = j; i < n; i += l2) {
				i1 = i + l1;
				t1 = u1 * x[i1] - u2 * y[i1];
				t2 = u1 * y[i1] + u2 * x[i1];
				x[i1] = x[i] - t1;
				y[i1] = y[i] - t2;
				x[i] += t1;
				y[i] += t2;
			}
			z = u1 * c1 - u2 * c2;
			u2 = u1 * c2 + u2 * c1;
			u1 = z;
		}
		c2 = sqrt((1.0 - c1) / 2.0);
		if (dir == 1)
			c2 = -c2;
		c1 = sqrt((1.0 + c1) / 2.0);
	}

	/* Scaling for forward transform */
	if (dir == 1) {
		for (i = 0; i < n; i++) {
			x[i] /= n;
			y[i] /= n;
		}
	}
}

static bool check_illegal_3D_coordinates(const DeepBreathFrameData* breathing_data) {

	DeepBreathConfig& user_cfg = DeepBreathConfig::getInstance();

	bool illegal_3d_coordinates = false;
	//check for 0,-0,0 3d coordinates.
	for (std::pair<stickers, cv::Vec3f*> sticker_elem : breathing_data->stickers_map_cm) {
		stickers s = sticker_elem.first;
		cv::Vec3f* d3_coor = sticker_elem.second;
		if (user_cfg.stickers_included[s] &&
			(*d3_coor)[0] == 0.0 && (*d3_coor)[1] == -0 && (*d3_coor)[2] == 0) {
			illegal_3d_coordinates = true;
			break;
		}
	}
	return illegal_3d_coordinates;
}

//static void get_3d_coordinates(const rs2::depth_frame& depth_frame, float x, float y, cv::Vec3f& output) {
//	float pixel[2] = { x, y };
//	float point[3]; // From point (in 3D)
//	auto dist = depth_frame.get_distance(pixel[0], pixel[1]);
//
//	rs2_intrinsics depth_intr = depth_frame.get_profile().as<rs2::video_stream_profile>().get_intrinsics(); // Calibration data
//
//	rs2_deproject_pixel_to_point(point, &depth_intr, pixel, dist);
//
//	//convert to cm
//	output[0] = float(point[0]) * 100.0;
//	output[1] = float(point[1]) * 100.0;
//	output[2] = float(point[2]) * 100.0;
//
//}
