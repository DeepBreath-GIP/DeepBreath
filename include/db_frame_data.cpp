
#include <fstream>
#include <librealsense2/rsutil.h>

#include "db_frame_data.hpp"
#include "db_camera.hpp"
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

DeepBreathFrameData::DeepBreathFrameData() :
	left(NULL),
	right(NULL),
	mid1(NULL),
	mid2(NULL),
	mid3(NULL),
	stickers_map_cm({ {stickers::left, &left_cm},
						{stickers::mid1, &mid1_cm},
						{stickers::mid2, &mid2_cm},
						{stickers::mid3, &mid3_cm},
						{stickers::right, &right_cm} }),
	dLM1(0.0), dLM2(0.0), dLM3(0.0),
	dRM1(0.0), dRM2(0.0), dRM3(0.0),
	dM1M2(0.0), dM1M3(0.0), dM2M3(0.0),
	dLR(0.0),
	distances_map_2d({ {distances::left_mid1, &dLM1},
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
	distances_map_3d({ {distances::left_mid1, &dLM1_depth},
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
	tetra_volume(0.0), reimann_volume(0.0),
	color_timestamp(0.0), depth_timestamp(0.0)
{}

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

void DeepBreathFrameData::CalculateVolumes(const rs2::points& points, const rs2::depth_frame& depth_frame)
{
	if (!left || !right || !mid3) return;

	DeepBreathConfig& user_cfg = DeepBreathConfig::getInstance();

	if (user_cfg.volume_type == TETRAHEDRON) {
		//tetrahedron volume:
		Tetrahedron tet(left_cm, right_cm, mid3_cm);
		tetra_volume = tet.volume();
	}
	else { //REIMANN
		Surface chest(points, depth_frame, *left, *right, *mid3);
		reimann_volume = chest.volume();
	}
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


DeepBreathFrameData::Surface::Surface(const rs2::points& points, const rs2::depth_frame& depth_frame, cv::Vec3f& left, cv::Vec3f& right, cv::Vec3f& mid3) {

	float left_x = left[0];
	float right_x = right[0];
	//y axis is pointed DOWN:
	float bottom_y = std::min({ left[1], right[1] });
	float top_y = mid3[1];

	bbox = BoundingBox(left_x, right_x, top_y, bottom_y);


	this->h = top_y - bottom_y;
	this->w = right_x - left_x;

	this->mat = new cv::Vec3f * [h];

	for (int i = 0; i < this->h; i++) {
		this->mat[i] = new cv::Vec3f[w];

		// Empirically we see that the depth on the right edge is zero on some of the points. We will avoid these points in Reimann sum calculations
		for (int j = 0; j < this->w; ++j) {
			cv::Vec3f p(0, 0, 0);
			get_3d_coordinates(depth_frame, j + left_x, i + bottom_y, p);
			this->mat[i][j] = p;
		}
	}
}

float DeepBreathFrameData::Surface::volume() {

	float total = 0;
	float dij = 0;	// avg. depth of centroid of [i, i+1] x [j, j+1]
	float Aij = 0;	//area of [i, i+1] x [j, j+1]

	//calculate

	for (int i = 0; i < this->h - 2; i++) {

		for (int j = 0; j < this->w - 2; ++j) {

			//current point i,j vals:
			float p_x = (this->mat[i][j])[0];
			float p_y = (this->mat[i][j])[1];
			float p_z = (this->mat[i][j])[2];

			//point to the right:
			float r_x = (this->mat[i][j + 1])[0];
			float r_y = (this->mat[i][j + 1])[1];
			float r_z = (this->mat[i][j + 1])[2];

			//point to the bottom:
			float b_x = (this->mat[i + 1][j])[0];
			float b_y = (this->mat[i + 1][j])[1];
			float b_z = (this->mat[i + 1][j])[2];

			//diagonal point:
			float d_x = (this->mat[i + 1][j + 1])[0];
			float d_y = (this->mat[i + 1][j + 1])[1];
			float d_z = (this->mat[i + 1][j + 1])[2];

			cv::Vec3f p(p_x, p_y, p_z);
			cv::Vec3f r(r_x, r_y, r_z);
			cv::Vec3f b(b_x, b_y, b_z);
			cv::Vec3f d(d_x, d_y, d_z);

			// TODO: Find implementation for centroid of quadrilateral
			cv::Vec3f center(
				(p_x + r_x + b_x + d_x) / 4,
				(p_y + r_y + b_y + d_y) / 4,
				(p_z + r_z + b_z + d_z) / 4
			);
			Aij = area(p, r, b, d);
			dij = center[2];

			total += dij * Aij;
		}
	}

	return total;
}

float DeepBreathFrameData::Surface::area(cv::Vec3f& a, cv::Vec3f& b, cv::Vec3f& c, cv::Vec3f& d) {

	//triangle abc:
	float abc_space = triangle_area(a, b, c);

	//triangle adc:
	float adc_space = triangle_area(a, d, c);

	return (abc_space + adc_space);

}


float triangle_area(cv::Vec3f& a, cv::Vec3f& b, cv::Vec3f& c) {
	//find angle between two edges (ab, ac)
	if (a == b || a == c || b == c) {
		return 0;
	}
	float area = 0;
	cv::Vec3f u(c[0] - a[0], c[1] - a[1], c[2] - a[2]);
	cv::Vec3f v(b[0] - a[0], b[1] - a[1], b[2] - a[2]);
	float dot_product = (u[0] * v[0]) + (u[1] * v[1]) + (u[2] * v[2]);
	float u_size = sqrt(pow(u[0], 2) + pow(u[1], 2) + pow(u[2], 2));
	float v_size = sqrt(pow(v[0], 2) + pow(v[1], 2) + pow(v[2], 2));
	//cos(x) = u * v / (|u||v|)
	float cos_x = 0;
	if (u_size * v_size != 0) {
		cos_x = dot_product / (u_size * v_size);
	}
	float x = acos(cos_x);
	area = (u_size * v_size * sin(x) / 2);
	return area;
}

void get_3d_coordinates(const rs2::depth_frame& depth_frame, float x, float y, cv::Vec3f& output) {
	float pixel[2] = { x, y };
	float point[3]; // From point (in 3D)
	auto dist = depth_frame.get_distance(pixel[0], pixel[1]);

	rs2_intrinsics depth_intr = depth_frame.get_profile().as<rs2::video_stream_profile>().get_intrinsics(); // Calibration data

	rs2_deproject_pixel_to_point(point, &depth_intr, pixel, dist);

	//convert to cm
	output[0] = float(point[0]) * 100.0;
	output[1] = float(point[1]) * 100.0;
	output[2] = float(point[2]) * 100.0;

}