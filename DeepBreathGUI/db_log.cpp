#include "db_log.hpp"
#include <librealsense2/rsutil.h>
#include <opencv2/opencv.hpp>

DeepBreathLog* DeepBreathLog::_log = nullptr;

void DeepBreathLog::createInstance(const char* filename, int num_of_markers, std::string D2units)
{
	assert(filename != nullptr);
	_log = new DeepBreathLog(filename, num_of_markers, D2units);
}

DeepBreathLog& DeepBreathLog::getInstance() {
	return *_log;
}


DeepBreathLog::DeepBreathLog(const char* filename, int num_of_markers, std::string D2units) {

	std::string name_prefix;
	if (filename) {
		//std::string file_name = filename;
		name_prefix = "file_log"; //file_name.substr(file_name.find("\\"), file_name.find("\."));
	}
	else {
		name_prefix = "live_camera_log";
	}
	auto t = std::time(nullptr);
	auto tm = *std::localtime(&t);
	std::ostringstream oss;
	oss << std::put_time(&tm, "%d-%m-%Y_%H-%M-%S");
	std::string log_name = name_prefix + "_" + oss.str() + ".csv";
	log_file.open(log_name);
	if (num_of_markers == 3) {
		log_file << LOG_TITLES_3_STICKERS(D2units);
	}
	else if (num_of_markers == 4) {
		log_file << LOG_TITLES_4_STICKERS(D2units);
	}
	else { //5
		log_file << LOG_TITLES_5_STICKERS(D2units);
	}
}
