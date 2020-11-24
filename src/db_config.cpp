#include "db_config.hpp"
#include <fstream>
#include <iostream>
#include <string>
#include <cassert>

DeepBreathConfig* DeepBreathConfig::_config = nullptr;

void DeepBreathConfig::createInstance(const char * config_filepath, std::string * config_err)
{
	assert(config_filepath != nullptr && config_err != nullptr);
	_config = new DeepBreathConfig(config_filepath, config_err);
}

DeepBreathConfig& DeepBreathConfig::getInstance() {
	return *_config;
}

//TODO: remove err
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

		//get dimension:
		//first line is a comment
		std::getline(config_file, line);
		line_num++;
		//dimension line:
		std::getline(config_file, line);
		if (line.compare("2") == 0) {
			this->dimension = dimension::D2;
		}
		else if (line.compare("3") == 0) {
			this->dimension = dimension::D3;
		}
		else {
			throw line_num;
		}
		line_num++;

		//get mode:
		//comment line:
		std::getline(config_file, line);
		line_num++;
		//mode line:
		std::getline(config_file, line);
		if (line.compare("D") == 0) {
			this->mode = graph_mode::DISTANCES;
		}
		else if (line.compare("F") == 0) {
			this->mode = graph_mode::FOURIER;
		}
		else if (line.compare("V") == 0) {
			this->mode = graph_mode::VOLUME;
			line_num++;
		}
		else if (line.compare("L") == 0) {
			this->mode = graph_mode::LOCATION;
		}
		else if (line.compare("N") == 0) {
			this->mode = graph_mode::NOGRAPH;
		}
		else {
			throw line_num;
		}
		line_num++;

		std::getline(config_file, line); //empty line
		line_num++;

		std::getline(config_file, line); //distances comment line
		line_num++;

		//get distances:
		//skip distances for Locations - UNCOMMENT TO SKIP
		//if (this->mode == graph_mode::LOCATION) {
		//	getline(config_file, line);
		//	line_num++;
		//	while (line.substr(0, 1).compare("#") != 0) {
		//		getline(config_file, line);
		//		line_num++;
		//	}
		//}
		//else { //include for other modes
			// get distances to include
			for (int distInt = distances::left_mid1; distInt != distances::ddummy; distInt++) {
				distances d = static_cast<distances>(distInt);
				std::getline(config_file, line);
				std::string val = line.substr(line.length() - 1, line.length());
				if (val.compare("y") == 0) {
					this->dists_included[d] = true;
				}
				else if (val.compare("n") == 0) {
					this->dists_included[d] = false;
				}
				else
					throw line_num;
				line_num++;
			}
		//}

		//get locations:
		getline(config_file, line);	// new line
		line_num++;
		getline(config_file, line);	//locations comment
		line_num++;
		//for Distances, Fourier, Volume, No Graph: Skip locations - UNCOMMENT TO SKIP
		//if (this->mode == graph_mode::DISTANCES
		//	|| this->mode == graph_mode::FOURIER
		//	|| this->mode == graph_mode::VOLUME
		//	|| this->mode == graph_mode::NOGRAPH) {

		//	getline(config_file, line); //first location line
		//	line_num++;
		//	while (line.substr(0, 1).compare("#") != 0) {
		//		getline(config_file, line);
		//		line_num++;
		//	} //stops by the first next comment line (number of stickers)
		//}
		//else {
			//get included locations:
			//(at this point, current line is the location comment, so first iteration will get first location)
			for (int stInt = stickers::left; stInt != stickers::sdummy; stInt++) {
				stickers s = static_cast<stickers>(stInt);
				std::getline(config_file, line);
				std::string val = line.substr(line.length() - 1, line.length());
				if (val.compare("y") == 0) {
					this->stickers_included[s] = true;
				}
				else if (val.compare("n") == 0) {
					this->stickers_included[s] = false;
				}
				else
					throw line_num;
				line_num++;
			}

			getline(config_file, line); //empty line
			line_num++;
		//}

		// get num of stickers
		getline(config_file, line);
		if (line.compare("3") == 0) {
			num_of_stickers = 3;
		}
		else if (line.compare("4") == 0) {
			num_of_stickers = 4;
		}
		else if (line.compare("5") == 0) {
			num_of_stickers = 5;
		}
		else
			throw line_num;
		line_num++;

		//skip lines to next comment (color of stickers)
		while (line.substr(0, 1).compare("#") != 0) {
			getline(config_file, line);
			line_num++;
		}

		//get sticker color
		getline(config_file, line);
		std::string c = line.substr(0, 1);
		if (c.compare("Y") == 0)
			color = sticker_color::YELLOW;
		else if (c.compare("B") == 0)
			color = sticker_color::BLUE;
		else if (c.compare("G") == 0)
			color = sticker_color::GREEN;
		else throw line_num;
		line_num++;

		//skip lines to next comment (units of 2D measure)
		while (line.substr(0, 1).compare("#") != 0) {
			getline(config_file, line);
			line_num++;
		}

		//get 2D measure unit
		getline(config_file, line);
		if (line.compare("cm") == 0)
			calc_2d_by_cm = true;
		else if (line.compare("pixel") == 0)
			calc_2d_by_cm = false;
		else
			throw line_num;
		line_num++;

		//skip lines to next comment (is use stickers)
		while (line.substr(0, 1).compare("#") != 0) {
			getline(config_file, line);
			line_num++;
		}
		//get if use stickers:
		getline(config_file, line);
		if (line.compare("y") == 0)
			is_stickers = true;
		else if (line.compare("n") == 0)
			is_stickers = false;
		else
			throw line_num;
		line_num++;

		//skip lines to next comment (volume type)
		while (line.substr(0, 1).compare("#") != 0) {
			getline(config_file, line);
			line_num++;
		}

		//get volume type:
		getline(config_file, line);
		if (line.compare("T") == 0)
			volume_type = TETRAHEDRON;
		else if (line.compare("R") == 0)
			volume_type = REIMANN;
		else
			throw line_num;
		line_num++;


		//TODO: update illegal cases
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
	this->dimension = dimension::D2;
	this->mode = graph_mode::DISTANCES;
	for (int distInt = distances::left_mid1; distInt != distances::ddummy; distInt++) {
		distances d = static_cast<distances>(distInt);
		this->dists_included[d] = false;
	}
	this->dists_included[distances::left_mid3] = true;
	this->dists_included[distances::right_mid3] = true;
	this->dists_included[distances::left_mid2] = true;
	this->dists_included[distances::right_mid2] = true;
	this->dists_included[distances::mid2_mid3] = true;
	this->num_of_stickers = 4;
	this->color = sticker_color::YELLOW;
	this->calc_2d_by_cm = false;
	this->is_stickers = true;
	this->volume_type = TETRAHEDRON;
}
