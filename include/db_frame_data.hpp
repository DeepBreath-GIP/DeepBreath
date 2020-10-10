#pragma once

#include <librealsense2/rs.hpp>
#include <opencv2/opencv.hpp>

#include "db_config.hpp"


/*	DeepBreathFrameData class
	Stores the processed data of a frame.
	@ circles : saves the centroids of the circles found representing the stickers.
		Each circle has x,y coordinates and depth, indexes of each are in the corresponding order.
		First circle is the frame center.
	@ left, right, mid1-3 : Pointers to the coordinates of the corresponding sticker, as following:
		left ---- mid1 ---- right
		  -					 -
			-	  mid2	   -
			  -			 -
				- mid3 -
		Values can be invalid if stickers do not exist. Careful!
	@ ***_cm : coordinates in cm.
	@ dAB : distance in pixels between A and B, while A,B are initials of the names of the stickers.
	@ dAB_depth : 3D distance in cm between A and B, while A,B are initials of the names of the stickers.
	@ average_2d_dist, average_3d_dist : average distances between stickers, in 2D and in 3D.
	@ ***_timestamp : timestamp of each frame.
*/
class DeepBreathFrameData {
public:
	std::vector<cv::Vec3f> circles; //(x,y,depth)
	cv::Vec3f *left, *right, *mid1, *mid2, *mid3; //(x,y,depth)
	//TODO: adding alternative coordinates in cm, choose one after deciding which is more accurate (dist by pixels or by cm(given by rs2_deproject))
	cv::Vec3f left_cm, right_cm, mid1_cm, mid2_cm, mid3_cm; //(x,y,depth)
	std::map<stickers, cv::Vec3f*> stickers_map_cm;
	float dLM1, dLM2, dLM3, dLR, dRM1, dRM2, dRM3, dM1M2, dM1M3, dM2M3;
	std::map<distances, float*> distances_map_2d;
	float dLM1_depth, dLM2_depth, dLM3_depth, dLR_depth, dRM1_depth, dRM2_depth, dRM3_depth, dM1M2_depth, dM1M3_depth, dM2M3_depth;
	std::map<distances, float*> distances_map_3d;
	//float dLR_depth, dML_depth, dMR_depth, dMD_depth, dDL_depth, dDR_depth;
	float average_2d_dist;
	float average_3d_dist;
	double color_timestamp;
	double depth_timestamp;
	double system_timestamp;
	unsigned long long frame_idx, color_idx, depth_idx;
	double system_color_timestamp, system_depth_timestamp;

	//ctor:
	DeepBreathFrameData() :
		left(NULL),
		right(NULL),
		mid1(NULL),
		mid2(NULL),
		mid3(NULL),
		stickers_map_cm({	{stickers::left, &left_cm},
							{stickers::mid1, &mid1_cm},
							{stickers::mid2, &mid2_cm},
							{stickers::mid3, &mid3_cm},
							{stickers::right, &right_cm} }),
		dLM1(0.0), dLM2(0.0), dLM3(0.0),
		dRM1(0.0), dRM2(0.0), dRM3(0.0),
		dM1M2(0.0), dM1M3(0.0), dM2M3(0.0),
		dLR(0.0),
		distances_map_2d({	{distances::left_mid1, &dLM1},
							{distances::left_mid2, &dLM2},
							{distances::left_mid3, &dLM3},
							{distances::left_right, &dLR},
							{distances::right_mid1, &dRM1},
							{distances::right_mid2, &dRM2},
							{distances::right_mid3, &dRM3},
							{distances::mid1_mid2, &dM1M2},
							{distances::mid1_mid3, &dM1M3},
							{distances::mid2_mid3, &dM2M3} }),
		dLM1_depth(0.0), dLM2_depth(0.0), dLM3_depth(0.0),
		dRM1_depth(0.0), dRM2_depth(0.0), dRM3_depth(0.0),
		dM1M2_depth(0.0), dM1M3_depth(0.0), dM2M3_depth(0.0),
		dLR_depth(0.0),
		distances_map_3d({	{distances::left_mid1, &dLM1_depth},
							{distances::left_mid2, &dLM2_depth},
							{distances::left_mid3, &dLM3_depth},
							{distances::left_right, &dLR_depth},
							{distances::right_mid1, &dRM1_depth},
							{distances::right_mid2, &dRM2_depth},
							{distances::right_mid3, &dRM3_depth},
							{distances::mid1_mid2, &dM1M2_depth},
							{distances::mid1_mid3, &dM1M3_depth},
							{distances::mid2_mid3, &dM2M3_depth} }),
		average_2d_dist(0.0), average_3d_dist(0.0),
		color_timestamp(0.0), depth_timestamp(0.0)
	{}

	//METHODS://

	/* Updates the locations of the stickers and validates the pointers to them. */
	void UpdateStickersLoactions();

	/* Calculates 2D distances between all stickers and their average. */
	void CalculateDistances2D();
	/* Calculates 3D distances between all stickers and their average. */
	void CalculateDistances3D();

	/* Gets the description of the frame in the following format:
		TODO: update format */
	void GetDescription();
	/* for a log with precision 2*/
};

