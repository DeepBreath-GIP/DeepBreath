#include "rb_aux.h"
#include "utilities.h"
#include "os.h"
#include <cstdio>
#include <string>
#include <ctime>
#include <sstream>
#include "db_config.hpp"

//#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>
//#include <CvPlot/cvplot.h> // in .h file



#define CALC_2D_DIST(name1,name2) { distance2D((*(name1))[0], (*(name1))[1], (*(name2))[0], (*(name2))[1]) }
#define CALC_3D_DIST(name1,name2) { distance3D(name1[0], name1[1], name1[2], name2[0], name2[1], name2[2]) }
#define COORDINATES_TO_STRING_CM(circle) (std::to_string(circle[0]) + ", " + std::to_string(circle[1]) + ", " + std::to_string(circle[2]))
#define COORDINATES_TO_STRING_PIXELS(circle) (std::to_string(circle[0][0]) + ", " + std::to_string(circle[0][1]))

/* Compare functions to sort the stickers: */

static struct compareCirclesByY {
	//returns true if c2's y is greater than c1's y.
	bool operator() (cv::Vec3f& c1, cv::Vec3f& c2) {
		return (c1[1] < c2[1]);
	}
} compareCirclesByYFunc;

static struct compareCirclesByX {
	//returns true if c2's x is greater than c1's x.
	bool operator() (cv::Vec3f& c1, cv::Vec3f& c2) {
		return (c1[0] < c2[0]);
	}
} compareCirclesByXFunc;

FrameManager::FrameManager(unsigned int n_frames, const char * frame_disk_path) :
	_n_frames(n_frames), _frame_disk_path(frame_disk_path), _oldest_frame_index(0), interval_active(false)
{
	manager_start_time = clock();
	_frame_data_arr = new BreathingFrameData*[_n_frames];
	for (unsigned int i = 0; i < _n_frames; i++) {
		_frame_data_arr[i] = NULL;
	}
}

FrameManager::~FrameManager()
{
	cleanup();
	if (_frame_data_arr != NULL) {
		free(_frame_data_arr);
	}
}


void FrameManager::reset() {
	frame_idx = 1;
	first_timestamp = NULL;
	_oldest_frame_index = 0;
	interval_active = false;
	manager_start_time = clock();
	for (unsigned int i = 0; i < _n_frames; i++) {
		_frame_data_arr[i] = NULL;
	}
}

int FrameManager::get_frames_array_size() {
	int c = 0;
	for (unsigned int i = 0; i < _n_frames; i++) {
		if (_frame_data_arr[i] != NULL) {
			c++;
		}
	}
	return c;
}

void FrameManager::process_frame(const rs2::video_frame& color_frame, const rs2::depth_frame& depth_frame)
{

	BreathingFrameData * breathing_data = new BreathingFrameData();

	identify_markers(color_frame, depth_frame, breathing_data);
	
	//if user config dimension is 3D, check for 0,-0,0 3d coordinates. dump such frames
	if (DeepBreathConfig::getInstance().dimension == dimension::D3) {
		if (check_illegal_3D_coordinates(breathing_data)) {
			frames_dumped_in_row++;
			logFile << "Warning: illegal 3D coordinates! frame was dumped.,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,"; //NOTE: NUMBER OF ',' CHARACTERS MUST REMAIN AS IS!
			if (frames_dumped_in_row >= 3) {								//This is required for transition to next columns in the log file! 
				logFile << '\n';
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
		logFile << ",,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,";
		//NOTE: NUMBER OF ',' CHARACTERS MUST REMAIN AS IS! This is required for transition to next columns in the log file!
	}
}

void FrameManager::identify_markers(const rs2::video_frame& color_frame, const rs2::depth_frame& depth_frame, BreathingFrameData* breathing_data)
{
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

void FrameManager::update_timestamps(const rs2::video_frame& color_frame, const rs2::depth_frame& depth_frame, BreathingFrameData* breathing_data)
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

void FrameManager::cleanup()
{
	if (_frame_data_arr != NULL) {

		for (unsigned int i = 0; i < _n_frames; i++) {
			if (_frame_data_arr[i] != NULL) {
				free(_frame_data_arr[i]);
			}
			_frame_data_arr[i] = NULL;
		}
	}
	logFile << "frames array cleanup...\n,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,";
	//NOTE: NUMBER OF ',' CHARACTERS MUST REMAIN AS IS! This is required for transition to next columns in the log file!
	frames_dumped_in_row = 0;
}

void FrameManager::add_frame_data(BreathingFrameData * frame_data)
{
	// delete last frame
	if (_frame_data_arr[_oldest_frame_index] != NULL) {
		free(_frame_data_arr[_oldest_frame_index]);
		_frame_data_arr[_oldest_frame_index] = NULL;
	}

	_frame_data_arr[_oldest_frame_index] = frame_data;
	_oldest_frame_index = (_oldest_frame_index + 1) % _n_frames;
}


void FrameManager::get_locations(stickers s, std::vector<cv::Point2d> *out) {
	if (DeepBreathConfig::getInstance().mode != graph_mode::LOCATION) {
		logFile << "Warning: get_locations was called in incompatible mode! (use L mode),,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,";
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

void FrameManager::get_dists(std::vector<cv::Point2d>* out) {
	if (DeepBreathConfig::getInstance().mode == graph_mode::LOCATION) {
		logFile << "Warning: get_dists was called in LOCATION mode!,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,";
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

long double FrameManager::no_graph() {
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

void FrameManager::activateInterval() {
	this->interval_active = true;
}

void FrameManager::deactivateInterval() {
	this->interval_active = false;
}

void BreathingFrameData::UpdateStickersLoactions()
{
	if (circles.size() < DeepBreathConfig::getInstance().num_of_stickers) return;
	if (circles.size() == 4) {
		// no mid1 sticker
		//sort vec by y:
		std::sort(circles.begin(), circles.end(), compareCirclesByYFunc);
		//sort 2 highest by x:
		std::sort(circles.begin(), circles.begin() + 2, compareCirclesByXFunc);

		left = &circles[0];
		right = &circles[1];
		mid2 = &circles[2];
		mid3 = &circles[3];
	}
	else {
		// assume 5 stickers, arranged in a T shape
		//sort vec by y:
		std::sort(circles.begin(), circles.end(), compareCirclesByYFunc);
		//sort 3 highest by x:
		std::sort(circles.begin(), circles.begin() + 3, compareCirclesByXFunc);

		left = &circles[0];
		right = &circles[2];
		mid1 = &circles[1];
		mid2 = &circles[3];
		mid3 = &circles[4];
	}
	
}

void BreathingFrameData::CalculateDistances2D()
{
	if (!left || !right || !mid2 || !mid3) return;
	if (DeepBreathConfig::getInstance().num_of_stickers == 5 && !mid1) return;

	if (DeepBreathConfig::getInstance().calc_2d_by_cm) {
		dLM2 = CALC_2D_DIST((&left_cm), (&mid2_cm));
		dLM3 = CALC_2D_DIST((&left_cm), (&mid3_cm));
		dLR = CALC_2D_DIST((&left_cm), (&right_cm));
		dRM2 = CALC_2D_DIST((&right_cm), (&mid2_cm));
		dRM3 = CALC_2D_DIST((&right_cm), (&mid3_cm)); 
		dM2M3 = CALC_2D_DIST((&mid2_cm), (&mid3_cm));
		if (DeepBreathConfig::getInstance().num_of_stickers == 5) {		//sticker mid1 exists
			dLM1 = CALC_2D_DIST((&left_cm), (&mid1_cm));
			dRM1 = CALC_2D_DIST((&right_cm), (&mid1_cm));
			dM1M2 = CALC_2D_DIST((&mid1_cm), (&mid2_cm));
			dM1M3 = CALC_2D_DIST((&mid1_cm), (&mid3_cm));
		}
	}
	else {	// calc by pixels
		dLM2 = CALC_2D_DIST(left, mid2);
		dLM3 = CALC_2D_DIST(left, mid3);
		dLR = CALC_2D_DIST(left, right);
		dRM2 = CALC_2D_DIST(right, mid2);
		dRM3 = CALC_2D_DIST(right, mid3);
		dM2M3 = CALC_2D_DIST(mid2, mid3);
		if (DeepBreathConfig::getInstance().num_of_stickers == 5) {		//sticker mid1 exists
			dLM1 = CALC_2D_DIST(left, mid1);
			dRM1 = CALC_2D_DIST(right, mid1);
			dM1M2 = CALC_2D_DIST(mid1, mid2);
			dM1M3 = CALC_2D_DIST(mid1, mid3);
		}
	}
	
	//calculate average:
	average_2d_dist = 0.0;
	int c = 0;
	for (std::pair<distances, bool> dist_elem : DeepBreathConfig::getInstance().dists_included) {
		distances dist = dist_elem.first;
		bool is_included = dist_elem.second;
		if (is_included) { //if distance is included in user_cfg
			average_2d_dist += *(distances_map_2d[dist]);
			c += 1;
		}
	}
		average_2d_dist = average_2d_dist / (1.0*c);
}



void BreathingFrameData::CalculateDistances3D()
{
	if (!left || !right || !mid2 || !mid3) return;
	if (DeepBreathConfig::getInstance().num_of_stickers == 5 && !mid1) return;

	dLM2_depth = CALC_3D_DIST(left_cm, mid2_cm); 
	dLM3_depth = CALC_3D_DIST(left_cm, mid3_cm); 
	dLR_depth = CALC_3D_DIST(left_cm, right_cm); 
	dRM2_depth = CALC_3D_DIST(right_cm, mid2_cm);
	dRM3_depth = CALC_3D_DIST(right_cm, mid3_cm);
	dM2M3_depth = CALC_3D_DIST(mid2_cm, mid3_cm);

	if (DeepBreathConfig::getInstance().num_of_stickers == 5) {		//sticker mid1 exists
		dLM1_depth = CALC_3D_DIST(left_cm, mid1_cm);
		dRM1_depth = CALC_3D_DIST(right_cm, mid1_cm);
		dM1M2_depth = CALC_3D_DIST(mid1_cm, mid2_cm);
		dM1M3_depth = CALC_3D_DIST(mid1_cm, mid3_cm);
	}

	//calculate average:
	average_3d_dist = 0.0;
	int c = 0;
	for (std::pair<distances, bool> dist_elem : DeepBreathConfig::getInstance().dists_included) {
		distances dist = dist_elem.first;
		bool is_included = dist_elem.second;
		if (is_included) { //if distance is included in user_cfg
			average_3d_dist += *(distances_map_3d[dist]);
			c += 1;
		}
	}

	average_3d_dist = average_3d_dist / (1.0*c);
}

void BreathingFrameData::GetDescription()
{
	const std::string d2method = (DeepBreathConfig::getInstance().calc_2d_by_cm) ? "cm" : "pixels";

	logFile << std::to_string(frame_idx) << "," << std::to_string(color_idx) << "," << std::to_string(depth_idx) << "," << std::to_string(color_timestamp) << "," <<
		std::to_string(depth_timestamp) << "," << std::to_string(system_color_timestamp) << "," << std::to_string(system_depth_timestamp) << "," <<
		std::to_string(system_timestamp) << "," << std::fixed << std::setprecision(2) << left_cm[0] << " " << std::fixed << std::setprecision(2) << left_cm[1] << " " <<
		std::fixed << std::setprecision(2) << left_cm[2] << "," << std::fixed << std::setprecision(2) << right_cm[0] << " " << std::fixed << std::setprecision(2) <<
		right_cm[1] << " " << std::fixed << std::setprecision(2) << right_cm[2] << ",";
	if (DeepBreathConfig::getInstance().num_of_stickers == 5) {
		logFile << std::fixed << std::setprecision(2) << mid1_cm[0] << " " << std::fixed << std::setprecision(2) << mid1_cm[1] << " " << std::fixed << std::setprecision(2) <<
			mid1_cm[2] << ",";
	}
	logFile << std::fixed << std::setprecision(2) << mid2_cm[0] << " " << std::fixed << std::setprecision(2) << mid2_cm[1] << " " << std::fixed << std::setprecision(2) <<
		mid2_cm[2] << "," << std::fixed << std::setprecision(2) << mid3_cm[0] << " " << std::fixed << std::setprecision(2) << mid3_cm[1] << " " << std::fixed <<
		std::setprecision(2) << mid3_cm[2] << "," << std::fixed << std::setprecision(2) << (*left)[0] << " " << std::fixed << std::setprecision(2) << (*left)[1] << "," <<
		std::setprecision(2) << (*right)[0] << " " << std::setprecision(2) << (*right)[1] << ",";
	if (DeepBreathConfig::getInstance().num_of_stickers == 5) logFile << std::fixed << std::setprecision(2) << (*mid1)[0] << " " << std::fixed << std::setprecision(2) << (*mid1)[1] << ",";
	logFile << std::fixed << std::setprecision(2) << (*mid2)[0] << " " << std::fixed << std::setprecision(2) << (*mid2)[1] << "," << std::fixed << std::setprecision(2) <<
		(*mid3)[0] << " " << std::fixed << std::setprecision(2) << (*mid3)[1] << ",";

	if (DeepBreathConfig::getInstance().num_of_stickers == 5)  logFile << std::fixed << std::setprecision(2) << dLM1 << ",";
	logFile << std::fixed << std::setprecision(2) << dLM2 << "," << std::fixed << std::setprecision(2) << dLM3 << "," << std::fixed << std::setprecision(2) << dLR <<
		",";
	if (DeepBreathConfig::getInstance().num_of_stickers == 5)  logFile << std::fixed << std::setprecision(2) << dRM1 << ",";
	logFile << std::fixed << std::setprecision(2) << dRM2 << "," << std::fixed << std::setprecision(2) << dRM3 << ",";
	if (DeepBreathConfig::getInstance().num_of_stickers == 5) {
		logFile << std::fixed << std::setprecision(2) << dM1M2 << "," << std::fixed << std::setprecision(2) << dM1M3 << ",";
	}
	logFile << std::fixed << std::setprecision(2) << dM2M3 << ",";

	if (DeepBreathConfig::getInstance().num_of_stickers == 5)  logFile << std::fixed << std::setprecision(2) << dLM1_depth << ",";
	logFile << std::fixed << std::setprecision(2) << dLM2_depth << "," << std::fixed << std::setprecision(2) << dLM3_depth << "," <<
		std::fixed << std::setprecision(2) << dLR_depth << ",";
	if (DeepBreathConfig::getInstance().num_of_stickers == 5)  logFile << std::fixed << std::setprecision(2) << dRM1_depth << ",";
	logFile << std::fixed << std::setprecision(2) << dRM2_depth << "," << std::setprecision(2) << dRM3_depth << ",";
	if (DeepBreathConfig::getInstance().num_of_stickers == 5) {
		logFile << std::fixed << std::setprecision(2) << dM1M2_depth << "," << std::fixed << std::setprecision(2) << dM1M3_depth << ",";
	}
	logFile << std::fixed << std::setprecision(2) << dM2M3_depth << "," << std::fixed << std::setprecision(6) << average_2d_dist << "," <<
		std::fixed << std::setprecision(6) << average_3d_dist << ",";
}

void GraphPlot::_plotFourier(std::vector<cv::Point2d>& points)
{
	normalize_distances(&points);
	std::vector<cv::Point2d> frequency_points;
	long double f = (calc_frequency_fft(&points, &frequency_points));
	axes.setXLim(std::pair<double, double>(0, 5));
	axes.setYLim(std::pair<double, double>(0, 5));
	axes.create<CvPlot::Series>(frequency_points, "-k");
	if (points.size() < NUM_OF_LAST_FRAMES * 0.5) f = 0;
	long double bpm = f * 60;

	const std::string bpm_title("Freq: " + std::to_string(f) + " | " + " BPM: " + std::to_string(bpm));
	axes.title(bpm_title);


	window->update();
}

void GraphPlot::_plotDists(std::vector<cv::Point2d>& points)
{
	//axes.setXLim(std::pair<double, double>(Dx_LOWER_BOUND, Dx_UPPER_BOUND));
	//axes.setYLim(std::pair<double, double>(lower, upper));
	
	axes.create<CvPlot::Series>(points, "-b");

	long double f;
	if (GET_FREQUENCY_BY_FFT) {
		normalize_distances(&points);
		f = (calc_frequency_fft(&points));
	}
	else {	// get_frequency_differently
		bool cm_units = (_dimension == dimension::D3) ? true : DeepBreathConfig::getInstance().calc_2d_by_cm;
		f = calc_frequency_differently(&points, cm_units);
	}
	if (points.size() < NUM_OF_LAST_FRAMES * 0.5) f = 0;
	long double bpm = f * 60;
	const std::string bpm_title("BPM: " + std::to_string(bpm));
	axes.title(bpm_title);
	window->update();
}

void GraphPlot::_plotLoc(std::vector<cv::Point2d>& points, const char * lineSpec)
{
	axes.create<CvPlot::Series>(points, lineSpec);
	window->update();
}

void GraphPlot::_plotNoGraph(std::vector<cv::Point2d>& points)
{
	long double f;
	if (GET_FREQUENCY_BY_FFT) {
		normalize_distances(&points);
		f = (calc_frequency_fft(&points));
	}
	else {	// get_frequency_differently
		bool cm_units = (_dimension == dimension::D3) ? true : DeepBreathConfig::getInstance().calc_2d_by_cm;
		f = calc_frequency_differently(&points, cm_units);
	}
	if (points.size() < NUM_OF_LAST_FRAMES * 0.5) f = 0;
	long double bpm = f * 60;
	
	const std::string bpm_title("Freq | BPM");

	cv::Mat1d mat(1, 2);
	mat(0, 0) = f;
	mat(0, 1) = bpm;
	axes = CvPlot::plotImage(mat);

	axes.title(bpm_title);
	window->update();
}

void GraphPlot::_init_plot_window()
{
	switch (_mode) {
	case graph_mode::DISTANCES:
		window = new CvPlot::Window("Distances", axes, 600, 800);
		break;
	case graph_mode::FOURIER:
		window = new CvPlot::Window("Fourier", axes, 600, 800);
		break;
	case graph_mode::LOCATION:
		window = new CvPlot::Window("Depth", axes, 600, 800);
		break;
	case graph_mode::NOGRAPH:
		window = new CvPlot::Window("BPM", axes, 600, 800);
		break;
	}


}

GraphPlot::GraphPlot(graph_mode mode, dimension dimension, clock_t start_time):
	_mode(mode), _dimension(dimension), first_plot(true) {
	clock_t current_system_time = clock();
	time_begin = (current_system_time - start_time) / double(CLOCKS_PER_SEC);
}

void GraphPlot::reset(clock_t start_time) {
	clock_t current_system_time = clock();
	time_begin = (current_system_time - start_time) / double(CLOCKS_PER_SEC);
	axes = CvPlot::makePlotAxes();
	delete window;
	first_plot = true;
}

void GraphPlot::plot(std::vector<cv::Point2d>& points, const char * lineSpec) {
	// TODO: Copy vector when we move this logic to the thread
	if (first_plot) {	
		_init_plot_window();
		first_plot = false;
	}

	if (_mode == graph_mode::FOURIER) {
		axes = CvPlot::makePlotAxes();
	}

	switch (_mode) {
	case graph_mode::DISTANCES:
		_plotDists(points);
		break;
	case graph_mode::FOURIER:
		_plotFourier(points);
		break;
	case graph_mode::LOCATION:
		_plotLoc(points, lineSpec);
		break;
	case graph_mode::NOGRAPH:
		_plotNoGraph(points);
	}
}

/* OLD FUNCTIONS: */

//functions used for debugging, might be useful for further needs

/*
void save_last_frame(const char* filename, const rs2::video_frame& frame) {
	static int frame_index = 0;
	static int frame_counter = 0;
	static std::string frame_filenames[NUM_OF_LAST_FRAMES] = { "" };

	std::string stream_desc{};
	std::string filename_base(filename);

	stream_desc = rs2_stream_to_string(frame.get_profile().stream_type());
	auto t = std::time(nullptr);
	auto tm = *std::localtime(&t);
	std::ostringstream oss;
	oss << std::put_time(&tm, "%d-%m-%Y %H-%M-%S");

	auto filename_png = filename_base + "_" + stream_desc + oss.str() + std::to_string(frame_counter) + ".png";

	// delete oldest frame file 
	if (frame_filenames[frame_index] != "") {
		const int result = remove(frame_filenames[frame_index].c_str());
		if (result != 0) {
			printf("remove(%s) failed. Error: %s\n", frame_filenames[frame_index].c_str(), strerror(errno)); // No such file or directory
			// TODO: throw exception
		}
	}

	rs2::save_to_png(filename_png.data(), frame.get_width(), frame.get_height(), frame.get_bytes_per_pixel(),
		frame.get_data(), frame.get_width() * frame.get_bytes_per_pixel());

	frame_filenames[frame_index] = filename_png;
	frame_index = (frame_index + 1) % NUM_OF_LAST_FRAMES;
	frame_counter++;
}
*/