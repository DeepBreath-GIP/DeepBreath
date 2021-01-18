#include <filesystem> 
#include <string>
#include "db_log.hpp"
#include "db_config.hpp"
#include <list>
#include <plog/Init.h>

namespace fs = std::filesystem;

plog::RollingFileAppender<DeepBreathCSVFormatter> DeepBreathLog::csv_logger("");
bool DeepBreathLog::first_init = true;

static std::string build_distance_headers(bool is_2d, unsigned int * num_headers) {

	auto user_cfg = DeepBreathConfig::getInstance();

	std::string unit = (user_cfg.calc_2d_by_cm ? "cm" : "pixels");
	std::string left_mid1_2d_dist = "left - mid1 2D distance " + unit;
	std::string left_mid2_2d_dist = "left - mid2 2D distance " + unit;
	std::string left_mid3_2d_dist = "left - mid3 2D distance " + unit;
	std::string left_right_2d_dist = "left - right 2D distance " + unit;
	std::string right_mid1_2d_dist = "right - mid1 2D distance " + unit;
	std::string right_mid2_2d_dist = "right - mid2 2D distance " + unit;
	std::string right_mid3_2d_dist = "right - mid3 2D distance " + unit;
	std::string mid1_mid2_2d_dist = "mid1 - mid2 2D distance " + unit;
	std::string mid2_mid3_2d_dist = "mid2 - mid3 2D distance " + unit;
	std::string mid1_mid3_2d_dist = "mid1 - mid3 2D distance " + unit;
	std::string average_2d_dist = "2D average distance " + unit;


	std::string left_mid1_3d_dist = "left - mid1 3D distance cm";
	std::string left_mid2_3d_dist = "left - mid2 3D distance cm";
	std::string left_mid3_3d_dist = "left - mid3 3D distance cm";
	std::string left_right_3d_dist = "left - right 3D distance cm";
	std::string right_mid1_3d_dist = "right - mid1 3D distance cm";
	std::string right_mid2_3d_dist = "right - mid2 3D distance cm";
	std::string right_mid3_3d_dist = "right - mid3 3D distance cm";
	std::string mid1_mid2_3d_dist = "mid1 - mid2 3D distance cm";
	std::string mid2_mid3_3d_dist = "mid2 - mid3 3D distance cm";
	std::string mid1_mid3_3d_dist = "mid1 - mid3 3D distance cm";
	std::string average_3d_dist = "3D average distance cm";


	std::list<std::string> dist_headers_2d_included;
	std::list<std::string> dist_headers_3d_included;
	for (auto dist_type : user_cfg.dists_included) {
		if (dist_type.second) {
			switch (dist_type.first) {
			case left_mid1:
				dist_headers_2d_included.push_back(left_mid1_2d_dist);
				if (!is_2d) {
					dist_headers_3d_included.push_back(left_mid1_3d_dist);
				}
				break;
			case left_mid2:
				dist_headers_2d_included.push_back(left_mid2_2d_dist);
				if (!is_2d) {
					dist_headers_3d_included.push_back(left_mid2_3d_dist);
				}
				break;
			case left_mid3:
				dist_headers_2d_included.push_back(left_mid3_2d_dist);
				if (!is_2d) {
					dist_headers_3d_included.push_back(left_mid3_3d_dist);
				}
				break;
			case left_right:
				dist_headers_2d_included.push_back(left_right_2d_dist);
				if (!is_2d) {
					dist_headers_3d_included.push_back(left_right_3d_dist);
				}
				break;
			case right_mid1:
				dist_headers_2d_included.push_back(right_mid1_2d_dist);
				if (!is_2d) {
					dist_headers_3d_included.push_back(right_mid1_3d_dist);
				}
				break;
			case right_mid2:
				dist_headers_2d_included.push_back(right_mid2_2d_dist);
				if (!is_2d) {
					dist_headers_3d_included.push_back(right_mid2_3d_dist);
				}
				break;
			case right_mid3:
				dist_headers_2d_included.push_back(right_mid3_2d_dist);
				if (!is_2d) {
					dist_headers_3d_included.push_back(right_mid3_3d_dist);
				}
				break;
			case mid1_mid2:
				dist_headers_2d_included.push_back(mid1_mid2_2d_dist);
				if (!is_2d) {
					dist_headers_3d_included.push_back(mid1_mid2_3d_dist);
				}
				break;
			case mid1_mid3:
				dist_headers_2d_included.push_back(mid1_mid3_2d_dist);
				if (!is_2d) {
					dist_headers_3d_included.push_back(mid1_mid3_3d_dist);
				}
				break;
			case mid2_mid3:
				dist_headers_2d_included.push_back(mid2_mid3_2d_dist);
				if (!is_2d) {
					dist_headers_3d_included.push_back(mid2_mid3_3d_dist);
				}
				break;
			}
		}
	}

	std::string dist_headers;
	bool is_first = true;
	*num_headers = 0;
	for (auto header : dist_headers_2d_included) {
		if (is_first) {
			dist_headers = header;
			is_first = false;
			(*num_headers)++;
		}
		else {
			dist_headers += "," + header;
			(*num_headers)++;
		}
	}
	for (auto header : dist_headers_3d_included) {
			dist_headers += "," + header;
			(*num_headers)++;
	}

	dist_headers += "," + average_2d_dist;
	(*num_headers)++;
	if (!is_2d) {
		dist_headers += "," + average_3d_dist;
		(*num_headers)++;
	}
	return dist_headers;
}

static std::string build_location_headers(int num_stickers, bool is_2d, bool is_cm, unsigned int* num_headers) {

	std::string unit = (is_cm ? "cm" : "pixels");
	std::string left_xy = "left(x y) " + unit;
	std::string right_xy = "right(x y) " + unit;
	std::string mid1_xy = "mid1(x y) " + unit;
	std::string mid2_xy = "mid2(x y) " + unit;
	std::string mid3_xy = "mid3(x y) " + unit;

	std::string left_xyz = "left(x y z) cm";
	std::string right_xyz = "right(x y z) cm";
	std::string mid1_xyz = "mid1(x y z) cm";
	std::string mid2_xyz = "mid2(x y z) cm";
	std::string mid3_xyz = "mid3(x y z) cm";

	std::string location_headers;
	switch (num_stickers) {
	case 3:
		location_headers = DeepBreathCSVFormatter::row_adder(left_xy, right_xy, mid3_xy);
		if (!is_2d) {
			location_headers = DeepBreathCSVFormatter::row_adder(location_headers, left_xyz, right_xyz, mid3_xyz);
		}
		
		break;
	case 4:
		location_headers = DeepBreathCSVFormatter::row_adder(left_xy, right_xy, mid2_xy, mid3_xy);
		if (!is_2d) {
			location_headers = DeepBreathCSVFormatter::row_adder(location_headers, left_xyz, right_xyz, mid2_xyz, mid3_xyz);
		}
		break;
	case 5:
		location_headers = DeepBreathCSVFormatter::row_adder(left_xy, right_xy, mid1_xy, mid2_xy, mid3_xy);
		if (!is_2d) {
			location_headers = DeepBreathCSVFormatter::row_adder(location_headers, left_xyz, right_xyz, mid1_xyz, mid2_xyz, mid3_xyz);
		}
		break;
	}

	*num_headers = is_2d ? num_stickers : (num_stickers * 2);
	return location_headers;
}

void DeepBreathLog::set_csv_headers() {
	
	std::string frame_idx = "Frame idx";
	std::string color_idx = "Color idx";
	std::string depth_idx = "Depth idx";
	std::string color_timestamp = "Color timestamp";
	std::string depth_timestamp = "Depth timestamp";
	std::string system_color_timestamp = "System color timestamp";
	std::string system_depth_timestamp = "System depth timestamp";
	std::string system_timestamp = "System timestamp";
	std::string first_headers = DeepBreathCSVFormatter::row_adder(frame_idx, color_idx, depth_idx,
		color_timestamp, depth_timestamp, system_color_timestamp, system_depth_timestamp, system_timestamp);
	unsigned int num_first_headers = 8;

	// According to configuration, set headers
	auto user_cfg = DeepBreathConfig::getInstance();
	auto mode = user_cfg.mode;
	auto num_stickers = user_cfg.num_of_stickers;
	auto dimension = user_cfg.dimension;
	auto is_cm = user_cfg.calc_2d_by_cm;
	auto volume_type = user_cfg.volume_type;

	unsigned int num_location_headers;
	std::string location_headers = build_location_headers(num_stickers, dimension == D2, is_cm, &num_location_headers);
	unsigned int num_distance_headers;
	std::string distance_headers = build_distance_headers(dimension == D2, &num_distance_headers);

	std::string all_headers = DeepBreathCSVFormatter::row_adder(first_headers, location_headers, distance_headers);
	unsigned int num_all_headers = num_first_headers + num_location_headers + num_distance_headers;

	if (mode == VOLUME) {
		switch(volume_type) {
		case TETRAHEDRON:
			all_headers = DeepBreathCSVFormatter::row_adder(all_headers, "Tetrahedron Volume");
			num_all_headers++;
			break;
		case REIMANN:
			all_headers = DeepBreathCSVFormatter::row_adder(all_headers, "Reimann Volume");
			num_all_headers++;
			break;
		}
	}

	switch (mode) {
	case FOURIER:
	case NOGRAPH:
	case DISTANCES:
	case VOLUME:
		std::string breath_rate_headers = DeepBreathCSVFormatter::row_adder(std::string("FPS"), std::string("Real #samples"), std::string("Frequency"), std::string("BPM"));
		all_headers = DeepBreathCSVFormatter::row_adder(all_headers, breath_rate_headers);
		num_all_headers += 4;
		break;
	}

	DeepBreathCSVFormatter::set_headers(all_headers);
	DeepBreathCSVFormatter::set_num_row_items(num_all_headers);
}

void DeepBreathLog::init(bool file_mode) {

	if (!fs::is_directory("logs") || !fs::exists("logs")) { // Check if log folder exists
		fs::create_directory("logs"); // create log folder
	}

	auto user_cfg = DeepBreathConfig::getInstance();
	auto mode = user_cfg.mode;

	std::string name_prefix;
	if (file_mode) {
		name_prefix = "logs\\file_log_" + DeepBreathConfig::graph_mode_to_string(mode);
	}
	else {
		name_prefix = "logs\\live_camera_log_" + DeepBreathConfig::graph_mode_to_string(mode);
	}
	auto t = std::time(nullptr);
	auto tm = *std::localtime(&t);
	std::ostringstream oss;
	oss << std::put_time(&tm, "%d-%m-%Y_%H-%M-%S");
	std::string log_name = name_prefix + "_" + oss.str() + ".csv";
	csv_logger.setFileName(log_name.c_str());

	DeepBreathLog::set_csv_headers();
	csv_logger.rollLogFiles();

	if (first_init) {
		plog::init(plog::info, &csv_logger);
		first_init = false;
	}
}

void DeepBreathLog::stop() {
	// We will change the file name to force the close of the file
	csv_logger.setFileName("");
}