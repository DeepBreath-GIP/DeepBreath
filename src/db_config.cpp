#include "db_config.hpp"
#include <fstream>
#include <string>
#include <cassert>

DeepBreathConfig* DeepBreathConfig::_config = nullptr;

const DeepBreathConfig& DeepBreathConfig::getInstance(const char* config_filepath, std::string* config_err) {
	if (_config == nullptr) {
		assert(config_filepath != nullptr && config_err != nullptr);
		_config = new DeepBreathConfig(config_filepath, config_err);
	}
	return *_config;
}

DeepBreathConfig::DeepBreathConfig(const char* config_filepath, std::string* err) {
	*err = "";
	std::ifstream config_file;

	config_file.open(config_filepath);
	if (!config_file.is_open()) {
		*err += "Unable to open config file.\nDefault setting used instead.";
		set_default();
		return;
	}
	int line_num = 1;
	try {
		std::string line;
		std::getline(config_file, line); //first line is a comment
		line_num++;
		//get dimension
		std::getline(config_file, line);
		if (line.compare("2") == 0) {
			DeepBreathConfig::dimension = dimension::D2;
		}
		else {
			if (line.compare("3") == 0) {
				DeepBreathConfig::dimension = dimension::D3;
			}
			else {
				throw line_num;
			}
		}
		line_num++;

		std::getline(config_file, line); //next line is a comment
		line_num++;
		//get mode
		std::getline(config_file, line);
		if (line.compare("D") == 0 || line.compare("F") == 0 || line.compare("N") == 0) {
			line_num++;
			DeepBreathConfig::mode = (line.compare("D") == 0) ? graph_mode::DISTANCES : (line.compare("F") == 0) ? graph_mode::FOURIER : graph_mode::NOGRAPH;
			std::getline(config_file, line); // new line
			line_num++;
			std::getline(config_file, line); // comment
			line_num++;
			// get distances to include
			for (int distInt = distances::left_mid1; distInt != distances::ddummy; distInt++) {
				distances d = static_cast<distances>(distInt);
				std::getline(config_file, line);
				std::string val = line.substr(line.length() - 1, line.length());
				if (val.compare("y") == 0) DeepBreathConfig::dists_included[d] = true;
				else {
					if (val.compare("n") == 0) DeepBreathConfig::dists_included[d] = false;
					else throw line_num;
				}
				line_num++;
			}

			// skip locations
			getline(config_file, line);	// new line
			line_num++;
			getline(config_file, line);	// #location:...
			line_num++;
			getline(config_file, line);
			line_num++;
		}
		else {
			if (line.compare("L") != 0) throw line_num;
			line_num++;
			DeepBreathConfig::mode = graph_mode::LOCATION;
			std::getline(config_file, line); // new line
			line_num++;
			std::getline(config_file, line); // comment
			line_num++;
			// skip distances
			getline(config_file, line);
			line_num++;
			while (line.substr(0, 1).compare("#") != 0) {
				getline(config_file, line);
				line_num++;
			}
			// get included stickers
			for (int stInt = stickers::left; stInt != stickers::sdummy; stInt++) {
				stickers s = static_cast<stickers>(stInt);
				std::getline(config_file, line);
				std::string val = line.substr(line.length() - 1, line.length());
				if (val.compare("y") == 0) DeepBreathConfig::stickers_included[s] = true;
				else {
					if (val.compare("n") == 0) DeepBreathConfig::stickers_included[s] = false;
					else throw line_num;
				}
				line_num++;
			}
		}

		while (line.substr(0, 1).compare("#") != 0) {
			getline(config_file, line);
			line_num++;
		}


		// get num of stickers
		getline(config_file, line);
		if (line.compare("4") == 0) num_of_stickers = 4;
		else {
			if (line.compare("5") == 0) num_of_stickers = 5;
			else throw line_num;
		}
		line_num++;

		while (line.substr(0, 1).compare("#") != 0) {
			getline(config_file, line);
			line_num++;
		}

		// get sticker color
		getline(config_file, line);
		std::string c = line.substr(0, 1);
		if (c.compare("Y") == 0)
			color = sticker_color::YELLOW;
		else if (c.compare("B") == 0)
			color = sticker_color::BLUE;
		//else if (c.compare("R") == 0) color = sticker_color::RED;
		else if (c.compare("G") == 0)
			color = sticker_color::GREEN;
		else throw line_num;
		line_num++;

		while (line.substr(0, 1).compare("#") != 0) {
			getline(config_file, line);
			line_num++;
		}

		// get 2Dmeasure unit
		getline(config_file, line);
		if (line.compare("cm") == 0)
			calc_2d_by_cm = true;
		else if (line.compare("pixel") == 0) calc_2d_by_cm = false;
		else throw line_num;
		line_num++;

		// check illegal use of sticker mid1
		// if num_of_stickers is 4, there is no mid1 sticker
		if (num_of_stickers == 4) {
			if (mode == graph_mode::LOCATION && stickers_included[stickers::mid1]) {
				*err += "Warning: location of mid1 was set to y, while number of stickers is 4. This location will be disregarded.";
				stickers_included[stickers::mid1] = false;
			}
			if (mode != graph_mode::LOCATION) {
				if (dists_included[distances::left_mid1] || dists_included[distances::mid1_mid2] ||
					dists_included[distances::mid1_mid3] || dists_included[distances::right_mid1]) {
					*err += "Warning: distance from mid1 was set to y, while number of stickers is 4. This distance will be disregarded.";
					dists_included[distances::left_mid1] = dists_included[distances::mid1_mid2] =
						dists_included[distances::mid1_mid3] = dists_included[distances::right_mid1] = false;
				}
			}
		}
	}
	// if an error occured while parsing, config.txt is illegal. use default setting
	catch (...) {
		*err += "Error while reading line " + std::to_string(line_num) + " in config.txt.\nDefault setting used instead.";
		set_default();
	}
}

void DeepBreathConfig::set_default() {
	DeepBreathConfig::dimension = dimension::D2;
	DeepBreathConfig::mode = graph_mode::DISTANCES;
	DeepBreathConfig::dists_included[distances::left_mid1] = DeepBreathConfig::dists_included[distances::left_mid2] = DeepBreathConfig::dists_included[distances::left_right] =
		DeepBreathConfig::dists_included[distances::mid1_mid2] = DeepBreathConfig::dists_included[distances::mid1_mid3] = DeepBreathConfig::dists_included[distances::right_mid1] =
		DeepBreathConfig::dists_included[distances::right_mid2] = false;
	DeepBreathConfig::dists_included[distances::left_mid3] = DeepBreathConfig::dists_included[distances::right_mid3] = DeepBreathConfig::dists_included[distances::mid2_mid3] = true;
	num_of_stickers = 5;
	color = sticker_color::YELLOW;
	calc_2d_by_cm = false;
}
