#include <string>
#include "db_log.hpp"
#include "db_config.hpp"
#include <list>

plog::RollingFileAppender<DeepBreathCSVFormatter> DeepBreathLog::csv_logger("");

static std::string build_distance_headers(bool is_2d) {

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
	for (auto header : dist_headers_2d_included) {
		if (is_first) {
			dist_headers = header;
			is_first = false;
		}
		else {
			dist_headers += "," + header;
		}
	}
	for (auto header : dist_headers_3d_included) {
			dist_headers += "," + header;
	}

	dist_headers += "," + average_2d_dist;
	dist_headers += "," + average_3d_dist;

	return dist_headers;
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

	// According to configuration, set headers
	auto user_cfg = DeepBreathConfig::getInstance();
	auto mode = user_cfg.mode;
	auto num_stickers = user_cfg.num_of_stickers;
	auto dimension = user_cfg.dimension;
	auto is_cm = user_cfg.calc_2d_by_cm;
	auto volume_type = user_cfg.volume_type;

	std::string unit = (is_cm ? "cm" : "pixels");
	std::string left_xy = "left(x y) " + unit;
	std::string right_xy = "right(x y) " + unit;
	std::string mid1_xy = "mid1(x y) " + unit;
	std::string mid2_xy = "mid2(x y) " + unit;
	std::string mid3_xy = "mid3(x y) " + unit;

	std::string left_xyz = "left(x y) cm";
	std::string right_xyz = "right(x y) cm";
	std::string mid1_xyz = "mid1(x y) cm";
	std::string mid2_xyz = "mid2(x y) cm";
	std::string mid3_xyz = "mid3(x y) cm";

	std::string location_headers;
	std::string distance_headers = build_distance_headers(dimension == D2);;

	switch (num_stickers) {
	case 3:
		switch (dimension) {
		case D2:
			location_headers = DeepBreathCSVFormatter::row_adder(left_xy, right_xy, mid3_xy);
			break;
		case D3:
			location_headers = DeepBreathCSVFormatter::row_adder(left_xyz, right_xyz, mid3_xyz);
			break;
		}
		break;
	case 4:
		switch (dimension) {
		case D2:
			location_headers = DeepBreathCSVFormatter::row_adder(left_xy, right_xy, mid2_xy, mid3_xy);
			break;
		case D3:
			location_headers = DeepBreathCSVFormatter::row_adder(left_xyz, right_xyz, mid2_xyz, mid3_xyz);
			break;
		}
		break;
	case 5:
		switch (dimension) {
		case D2:
			location_headers = DeepBreathCSVFormatter::row_adder(left_xy, right_xy, mid1_xy, mid2_xy, mid3_xy);
			break;
		case D3:
			location_headers = DeepBreathCSVFormatter::row_adder(left_xyz, right_xyz, mid1_xyz, mid2_xyz, mid3_xyz);
			break;
		}
		break;
	}

	std::string all_headers = DeepBreathCSVFormatter::row_adder(first_headers, location_headers, distance_headers);
	std::string breath_rate_headers = DeepBreathCSVFormatter::row_adder(std::string("FPS"), std::string("Real #samples"), std::string("Frequency"), std::string("BPM"));

	if (mode == VOLUME) {
		std::string volume_headers;

		switch(volume_type) {
		case TETRAHEDRON:
			volume_headers = "Tetrahedron Volume";
			break;
		case REIMANN:
			volume_headers = "Reimann Volume";
			break;
		}
		all_headers = DeepBreathCSVFormatter::row_adder(all_headers, volume_headers);
	}

	switch (mode) {
	case FOURIER:
	case NOGRAPH:
	case DISTANCES:
	case VOLUME:
		all_headers = DeepBreathCSVFormatter::row_adder(all_headers, breath_rate_headers);
		break;
	}

	DeepBreathCSVFormatter::set_headers(all_headers);
}

void DeepBreathLog::init(bool file_mode) {

	std::string name_prefix;
	if (file_mode) {
		name_prefix = "file_log"; 
	}
	else {
		name_prefix = "live_camera_log";
	}
	auto t = std::time(nullptr);
	auto tm = *std::localtime(&t);
	std::ostringstream oss;
	oss << std::put_time(&tm, "%d-%m-%Y_%H-%M-%S");
	std::string log_name = name_prefix + "_" + oss.str() + ".csv";
	csv_logger.setFileName(log_name.c_str());

	DeepBreathLog::set_csv_headers();

	csv_logger.rollLogFiles();
}

void DeepBreathLog::stop() {
	// We will change the file name to force the close of the file
	csv_logger.setFileName("");
}