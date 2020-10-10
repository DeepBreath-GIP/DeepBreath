
#include <fstream>

#include "db_frame_data.hpp"
#include "db_log.hpp"

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


/* calculate distacnes functions:*/

static float distance2D(float x, float y, float a, float b) {
	return sqrt(pow(x - a, 2) + pow(y - b, 2));
}

static float distance3D(float x, float y, float z, float a, float b, float c) {
	return sqrt(pow(x - a, 2) + pow(y - b, 2) + pow(z - c, 2));
}

/*	METHODS:	*/

void DeepBreathFrameData::UpdateStickersLoactions()
{
	if (circles.size() < DeepBreathConfig::getInstance().num_of_stickers)
		return;

	//3 stickers: (no mid1, mid2)
	//left		right
	//
	//	   mid3
	if (circles.size() == 3) {
		//sort vec by y:
		std::sort(circles.begin(), circles.end(), compareCirclesByYFunc);
		//sort 2 highest by x:
		std::sort(circles.begin(), circles.begin() + 2, compareCirclesByXFunc);

		left = &circles[0];
		right = &circles[1];
		mid3 = &circles[2];
	}
	//4 stickers: (no mid1)
	//left		right
	//
	//	   mid2
	//	   mid3
	else if (circles.size() == 4) {
		//sort vec by y:
		std::sort(circles.begin(), circles.end(), compareCirclesByYFunc);
		//sort 2 highest by x:
		std::sort(circles.begin(), circles.begin() + 2, compareCirclesByXFunc);

		left = &circles[0];
		right = &circles[1];
		mid2 = &circles[2];
		mid3 = &circles[3];
	}
	//5 stickers: additional stickers beyond the 5 first are ignored, therefore user MUST use correctly
	//left		right
	//	   mid1
	//	   mid2
	//	   mid3
	else {
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

void DeepBreathFrameData::CalculateDistances2D()
{
	if (!left || !right || !mid3) return;
	int num_of_stickers = DeepBreathConfig::getInstance().num_of_stickers;
	if ((num_of_stickers == 5 && (!mid1 || !mid2))
		|| (num_of_stickers == 4 && !mid2))
		return;

	if (DeepBreathConfig::getInstance().calc_2d_by_cm) {
		//calculate left, right and mid3 (mutual distances in all cases):
		dLM3 = CALC_2D_DIST((&left_cm), (&mid3_cm));
		dLR = CALC_2D_DIST((&left_cm), (&right_cm));
		dRM3 = CALC_2D_DIST((&right_cm), (&mid3_cm));
		//calculate mid2 for 4 or 5 stickers:
		if (num_of_stickers == 4 || num_of_stickers == 5) {
			dLM2 = CALC_2D_DIST((&left_cm), (&mid2_cm));
			dRM2 = CALC_2D_DIST((&right_cm), (&mid2_cm));
			dM2M3 = CALC_2D_DIST((&mid2_cm), (&mid3_cm));
		}
		if (num_of_stickers == 5) { //5 stickers only: calculate mid1, mid2
			dLM1 = CALC_2D_DIST((&left_cm), (&mid1_cm));
			dRM1 = CALC_2D_DIST((&right_cm), (&mid1_cm));
			dM1M2 = CALC_2D_DIST((&mid1_cm), (&mid2_cm));
			dM1M3 = CALC_2D_DIST((&mid1_cm), (&mid3_cm));
		}
	}
	else {	// calc by pixels
		//calculate left, right and mid3 (mutual distances in all cases):
		dLM3 = CALC_2D_DIST(left, mid3);
		dLR = CALC_2D_DIST(left, right);
		dRM3 = CALC_2D_DIST(right, mid3);
		//calculate mid2 for 4 or 5 stickers:
		if (num_of_stickers == 4 || num_of_stickers == 5) {
			dLM2 = CALC_2D_DIST(left, mid2);
			dRM2 = CALC_2D_DIST(right, mid2);
			dM2M3 = CALC_2D_DIST(mid2, mid3);
		}
		if (num_of_stickers == 5) { //5 stickers only: calculate mid1, mid2
			dLM1 = CALC_2D_DIST(left, mid1);
			dRM1 = CALC_2D_DIST(right, mid1);
			dM1M2 = CALC_2D_DIST(mid1, mid2);
			dM1M3 = CALC_2D_DIST(mid1, mid3);
		}
	}

	//calculate average:
	average_2d_dist = 0.0;
	int count = 0;
	for (std::pair<distances, bool> dist_elem : DeepBreathConfig::getInstance().dists_included) {
		distances dist = dist_elem.first;
		bool is_included = dist_elem.second;
		if (is_included) { //if distance is included in user_cfg
			average_2d_dist += *(distances_map_2d[dist]);
			count += 1;
		}
	}
	average_2d_dist = average_2d_dist / (1.0 * count);
}



void DeepBreathFrameData::CalculateDistances3D()
{
	if (!left || !right || !mid3) return;
	int num_of_stickers = DeepBreathConfig::getInstance().num_of_stickers;
	if ((num_of_stickers == 5 && (!mid1 || !mid2))
		|| (num_of_stickers == 4 && !mid2))
		return;

	//calculate left, right and mid3 (mutual distances in all cases):
	dLM3_depth = CALC_3D_DIST(left_cm, mid3_cm);
	dLR_depth = CALC_3D_DIST(left_cm, right_cm);
	dRM3_depth = CALC_3D_DIST(right_cm, mid3_cm);
	//calculate mid2 for 4 or 5 stickers:
	if (num_of_stickers == 4 || num_of_stickers == 5) {
		dLM2_depth = CALC_3D_DIST(left_cm, mid2_cm);
		dRM2_depth = CALC_3D_DIST(right_cm, mid2_cm);
		dM2M3_depth = CALC_3D_DIST(mid2_cm, mid3_cm);
	}
	if (num_of_stickers == 5) { //5 stickers only: calculate mid1, mid2
		dLM1_depth = CALC_3D_DIST(left_cm, mid1_cm);
		dRM1_depth = CALC_3D_DIST(right_cm, mid1_cm);
		dM1M2_depth = CALC_3D_DIST(mid1_cm, mid2_cm);
		dM1M3_depth = CALC_3D_DIST(mid1_cm, mid3_cm);
	}

	//calculate average:
	average_3d_dist = 0.0;
	int count = 0;
	for (std::pair<distances, bool> dist_elem : DeepBreathConfig::getInstance().dists_included) {
		distances dist = dist_elem.first;
		bool is_included = dist_elem.second;
		if (is_included) { //if distance is included in user_cfg
			average_3d_dist += *(distances_map_3d[dist]);
			count += 1;
		}
	}

	average_3d_dist = average_3d_dist / (1.0 * count);
}

void DeepBreathFrameData::GetDescription()
{
	DeepBreathLog& log = DeepBreathLog::getInstance();
	assert(log); //log instance must be initiated before frame processing (i.e. "start camera" or "load file" before cv notify)

	const std::string d2method = (DeepBreathConfig::getInstance().calc_2d_by_cm) ? "cm" : "pixels";

	log.log_file << std::to_string(frame_idx) << ","
		<< std::to_string(color_idx) << "," << std::to_string(depth_idx)
		<< "," << std::to_string(color_timestamp) << "," << std::to_string(depth_timestamp) << ","
		<< std::to_string(system_color_timestamp) << "," << std::to_string(system_depth_timestamp) << ","
		<< std::to_string(system_timestamp) << ","
		<< std::fixed << std::setprecision(2) << left_cm[0] << " "
		<< std::fixed << std::setprecision(2) << left_cm[1] << " "
		<< std::fixed << std::setprecision(2) << left_cm[2] << ","
		<< std::fixed << std::setprecision(2) << right_cm[0] << " "
		<< std::fixed << std::setprecision(2) << right_cm[1] << " "
		<< std::fixed << std::setprecision(2) << right_cm[2] << ",";

	int num_of_markers = DeepBreathConfig::getInstance().num_of_stickers;

	if (num_of_markers == 5) {
		log.log_file << std::fixed << std::setprecision(2) << mid1_cm[0] << " "
			<< std::fixed << std::setprecision(2) << mid1_cm[1] << " "
			<< std::fixed << std::setprecision(2) << mid1_cm[2] << ",";
	}
	if (num_of_markers == 4 || num_of_markers == 5) {
		log.log_file << std::fixed << std::setprecision(2) << mid2_cm[0] << " "
			<< std::fixed << std::setprecision(2) << mid2_cm[1] << " "
			<< std::fixed << std::setprecision(2) << mid2_cm[2] << ",";
	}
	//any case, log mid 3:
	log.log_file << std::fixed << std::setprecision(2) << mid3_cm[0] << " "
		<< std::fixed << std::setprecision(2) << mid3_cm[1] << " "
		<< std::fixed << std::setprecision(2) << mid3_cm[2] << ","
		<< std::fixed << std::setprecision(2) << (*left)[0] << " "
		<< std::fixed << std::setprecision(2) << (*left)[1] << ","
		<< std::setprecision(2) << (*right)[0] << " " << std::setprecision(2) << (*right)[1] << ",";

	if (num_of_markers == 5) {
		log.log_file << std::fixed << std::setprecision(2) << (*mid1)[0] << " "
			<< std::fixed << std::setprecision(2) << (*mid1)[1] << ",";
	}
	if (num_of_markers == 4 || num_of_markers == 5) {
		log.log_file << std::fixed << std::setprecision(2) << (*mid2)[0] << " "
			<< std::fixed << std::setprecision(2) << (*mid2)[1] << ",";
	}
	log.log_file << std::fixed << std::setprecision(2) << (*mid3)[0] << " "
		<< std::fixed << std::setprecision(2) << (*mid3)[1] << ",";

	if (num_of_markers == 5) {
		log.log_file << std::fixed << std::setprecision(2) << dLM1 << ",";
	}
	if (num_of_markers == 4 || num_of_markers == 5) {
		log.log_file << std::fixed << std::setprecision(2) << dLM2 << ",";
	}
	log.log_file << std::fixed << std::setprecision(2) << dLM3 << ","
		<< std::fixed << std::setprecision(2) << dLR << ",";

	if (num_of_markers == 5) {
		log.log_file << std::fixed << std::setprecision(2) << dRM1 << ",";
	}
	if (num_of_markers == 4 || num_of_markers == 5) {
		log.log_file << std::fixed << std::setprecision(2) << dRM2 << ",";
	}
	log.log_file << std::fixed << std::setprecision(2) << dRM3 << ",";

	if (num_of_markers == 5) {
		log.log_file << std::fixed << std::setprecision(2) << dM1M2 << ","
			<< std::fixed << std::setprecision(2) << dM1M3 << ",";
	}
	if (num_of_markers == 4 || num_of_markers == 5) {
		log.log_file << std::fixed << std::setprecision(2) << dM2M3 << ",";
	}

	if (num_of_markers == 5) {
		log.log_file << std::fixed << std::setprecision(2) << dLM1_depth << ",";
	}
	if (num_of_markers == 4 || num_of_markers == 5) {
		log.log_file << std::fixed << std::setprecision(2) << dLM2_depth << ",";
	}
	log.log_file << std::fixed << std::setprecision(2) << dLM3_depth << ",";


	log.log_file << std::fixed << std::setprecision(2) << dLR_depth << ",";

	if (num_of_markers == 5) {
		log.log_file << std::fixed << std::setprecision(2) << dRM1_depth << ",";
	}
	if (num_of_markers == 4 || num_of_markers == 5) {
		log.log_file << std::fixed << std::setprecision(2) << dRM2_depth << ",";
	}
	log.log_file << std::setprecision(2) << dRM3_depth << ",";

	if (num_of_markers == 5) {
		log.log_file << std::fixed << std::setprecision(2) << dM1M2_depth << ","
			<< std::fixed << std::setprecision(2) << dM1M3_depth << ",";
	}
	if (num_of_markers == 4 || num_of_markers == 5) {
		log.log_file << std::fixed << std::setprecision(2) << dM2M3_depth << ",";
	}
	log.log_file << std::fixed << std::setprecision(6) << average_2d_dist << ","
		<< std::fixed << std::setprecision(6) << average_3d_dist << ",";
}
