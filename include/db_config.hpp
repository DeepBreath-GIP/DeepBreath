#pragma once

#include <map>

//file path should be relative to GUI's root folder
#define CONFIG_FILEPATH "config.txt"

enum sticker_color {
	YELLOW,
	BLUE,
	GREEN,
	RED
};

enum dimension {
	D2,
	D3
};

enum graph_mode {
	DISTANCES,
	LOCATION,
	FOURIER,
	VOLUME,
	NOGRAPH
};

enum stickers {
	left,
	mid1,
	mid2,
	mid3,
	right,
	sdummy // needed for enum iteration
};

enum distances {
	left_mid1,
	left_mid2,
	left_mid3,
	left_right,
	right_mid1,
	right_mid2,
	right_mid3,
	mid1_mid2,
	mid1_mid3,
	mid2_mid3,
	ddummy // needed for enum iteration
};

/*
	Configuration details extracted from config.txt
	@ mode: indeicates the kind of graph to be presented.
		distances - tracking a given set of distances. bpm will be calculated using the average of said set.
		location - tracking the location of a given set of stickers. (TODO: no bpm calculated for this mode?)
	@ stickers_included: set of stickers to include.
		for mode distances, TODO: any sticker required for an included distance? or it wont be used, tbd
		for mode location, all stickers of which the location is requested.
	@ dists_included: set of distances to be tracked
		only used in mode distances.
*/
class DeepBreathConfig {
public:

	//TODO: Remove config_err
	static void createInstance(const char* config_filepath = nullptr, std::string* config_err = nullptr);
	static const DeepBreathConfig& getInstance();

	int num_of_stickers;
	bool calc_2d_by_cm; //if false, calculate by pixels
	dimension dimension;
	graph_mode mode;
	std::map<stickers, bool> stickers_included;
	std::map<distances, bool> dists_included;
	sticker_color color;

	// set configuration to default. used in case of en illegal config file.
	void set_default();

	// TODO: Free the _config and call it in rb-gui.
	void release();
protected:

	/*
	Construct a Config
	*/
	DeepBreathConfig(const char* config_filepath, std::string* config_err);

	static DeepBreathConfig* _config;

};
