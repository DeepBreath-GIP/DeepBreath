#pragma once

#include <librealsense2/rs.hpp>
#include <opencv2/opencv.hpp>

#include "db_config.hpp"
#include "db_frame_data.hpp"

#define NUM_OF_LAST_FRAMES 256 
#define GET_FREQUENCY_BY_FFT true	//if false, use get_frequency_differently

/* FrameManager class.
	Global singleton.
	Manages all frames and the memory required for them.
*/
class DeepBreathFrameManager {

public:

	static DeepBreathFrameManager& getInstance();

	clock_t manager_start_time;

	//dtor
	~DeepBreathFrameManager();

	/* reset frame manager for another run (switch between files or between live camera to file and vice versa */
	void reset();

	int get_frames_array_size();

	/**
	 * Processes a video color frame
	 *
	 * @param frame - a video frame from the camera
	 *
	 */
	void process_frame(const rs2::video_frame& color_frame, const rs2::depth_frame& depth_frame);

	/**
	 * Calculate breath rate of the NUM_OF_LAST_FRAMES samples processed.
	 */
	long double calculate_breath_rate();

	/**
	 * Get FPS:
	 */
	float get_fps();

	/* Turn interval activity on/off: */
	void activateInterval();
	void deactivateInterval();

	void log_last_frame_data();
	void log_breathing_data();
protected:

	//ctor
	DeepBreathFrameManager(unsigned int n_frames = NUM_OF_LAST_FRAMES, const char * frame_disk_path = NULL);

	static DeepBreathFrameManager* _frame_manager;

private:

	long double _calc_bpm(std::vector<cv::Point2d>& points);

	/**
	 * Cleans all allocated resources
	 */
	void cleanup();

	/**
	 * Add frame_data to collection of frame datas.
	 * NOTE: only last n_frames saved so the oldest frame_data will be deleted
	 */
	void add_frame_data(DeepBreathFrameData * frame_data);

	/**
	 * Add the data arrived to the graph according to the graph mode as set in config.
	 */
	void add_data_to_graph(DeepBreathFrameData * frame_data);

	/* Identify marker points: stickers or nipples and bellybutton image recognition. */
	void identify_markers(const rs2::video_frame& color_frame, const rs2::depth_frame& depth_frame, DeepBreathFrameData* breathing_data);

	/* Update frames timestamps. */
	void update_timestamps(const rs2::video_frame& color_frame, const rs2::depth_frame& depth_frame, DeepBreathFrameData* breathing_data);

	/**
	* To be used in L mode (for plotting locations of stickers)
	* TODO: for now, return only z coordinate (depth)
	* returns system_timestamp and according depth of sticker s for every frame received in the last 15 seconds
	* if called in L mode, no points are pushed to vector out
	*/
	void get_locations(stickers s, std::vector<cv::Point2d> *out);

	/**
	* To be used in D mode (for plotting avg distance of stickers)
	* To be used in F mode for calculating fft
	* returns system_timestamp and according avg distance of every frame received in the last 15 seconds
	* the avg distance is calculated only for distances set to true in user_cfg.dists_included
	* if called in L mode, no points are pushed to vector out
	*/
	void get_dists(std::vector<cv::Point2d> *out);

	/**
	* To be used in V mode (for plotting d volumes)
	* returns system_timestamp and according difference of volume of every two following frames received in the last 15 seconds
	* the volumes are calculated according to three markers: left, right, mid3 + another permanent point in the background.
	* if called in L mode, no points are pushed to vector out
	*/
	void get_volumes(std::vector<cv::Point2d> *out);

	/**
	* To be used in N mode (for calculating frequency and BPM without generating any graphs)
	* returns frequency calculated using get_frequency_fft if GET_FREQUENCY_BY_FFT. using get_frequency_differently otherwise.
	* the avg distances used for crequency calculation are the  distances set to true in user_cfg.dists_included
	*/
	long double no_graph();

	void calc_frequency_fft(std::vector<cv::Point2d>* samples, std::vector<cv::Point2d>* out_frequencies = NULL);

	int frame_idx = 1;
	double first_timestamp = NULL;
	unsigned int _n_frames;
	unsigned int _oldest_frame_index;
	DeepBreathFrameData** _frame_data_arr;
	const char* _frame_disk_path;
	bool interval_active;
	int frames_dumped_in_row = 0; //reinitialized after cleanup

	// breathing data
	float fps;
	float real_num_samples;
	float frequency;
	float bpm;
};
