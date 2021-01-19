#pragma once

#include <librealsense2/rs.hpp>
#include <opencv2/opencv.hpp>
//#include <cv-helpers.hpp>

#include "db_config.hpp"

//#define CALC_TRIANGLE_CENTER_COORDS(a,b,c) { (a[0] + b[0] + c[0]) / 3, (a[1] + b[1] + c[1]) / 3, (a[2] + b[2] + c[2]) / 3 }

float triangle_area(cv::Vec3f& left, cv::Vec3f& right, cv::Vec3f& bottom);
void get_3d_coordinates(const rs2::depth_frame& depth_frame, float x, float y, cv::Vec3f& output);


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
	//alternative coordinates in cm, choose one after deciding which is more accurate (dist by pixels or by cm(given by rs2_deproject))
	cv::Vec3f left_cm, right_cm, mid1_cm, mid2_cm, mid3_cm; //(x,y,depth)
	std::map<stickers, cv::Vec3f*> stickers_map_cm;
	float dLM1, dLM2, dLM3, dLR, dRM1, dRM2, dRM3, dM1M2, dM1M3, dM2M3;
	std::map<distances, float*> distances_map_2d;
	float dLM1_depth, dLM2_depth, dLM3_depth, dLR_depth, dRM1_depth, dRM2_depth, dRM3_depth, dM1M2_depth, dM1M3_depth, dM2M3_depth;
	std::map<distances, float*> distances_map_3d;
	//float dLR_depth, dML_depth, dMR_depth, dMD_depth, dDL_depth, dDR_depth;
	float average_2d_dist;
	float average_3d_dist;

	//Volume related:
	cv::Vec3f** scatter;
	int scatter_width;
	int scatter_height;
	float tetra_volume;
	float reimann_volume;

	//Timestamps & indexing:
	double color_timestamp;
	double depth_timestamp;
	double system_timestamp;
	unsigned long long frame_idx, color_idx, depth_idx;
	double system_color_timestamp, system_depth_timestamp;

	//ctor:
	DeepBreathFrameData();

	//METHODS://

	/* Updates the locations of the stickers and validates the pointers to them. */
	void UpdateStickersLoactions();

	//void getDepths();

	/* Calculates 2D distances between all stickers and their average. */
	void CalculateDistances2D();
	/* Calculates 3D distances between all stickers and their average. */
	void CalculateDistances3D();

	/* Calculates volumes based on markers' distances and depths.
		Calculates both volumes, while taking consideration only of 3 markers (left, right, mid3).
		NOTE: Call function AFTER 3D Distances calculation. */
	void CalculateVolumes(const rs2::depth_frame& depth_frame);

	/* Log all relevant frame data*/
	void log();
	/* for a log with precision 2*/

private:

	void general_log();
	void locations_log(int num_stickers, bool is_2d, bool is_cm);
	void distances_log(bool is_2d, bool is_cm);
	void volume_log();

	/* Tetrahedron for Volume calculations: */
	class Tetrahedron {
		cv::Vec3f left, right, bottom, head, middle;

	public:
		Tetrahedron(cv::Vec3f& left, cv::Vec3f& right, cv::Vec3f& bottom) :
			left(left), right(right), bottom(bottom), head(cv::Vec3f(0, 0, 0)), middle(cv::Vec3f(0, 0, 0))
		{
			for (int i = 0; i < 3; i++) {
				middle[i] = (left[i] + right[i] + bottom[i]) / 3;
			}

			//find normal to plane 
			cv::Vec3f u(bottom[0] - left[0], bottom[1] - left[1], bottom[2] - left[2]);
			cv::Vec3f v(right[0] - left[0], right[1] - left[1], right[2] - left[2]);
			cv::Vec3f n(0, 0, 0);
			//n = u x v
			n[0] = u[1] * v[2] - u[2] * v[1];
			n[1] = u[2] * v[0] - u[0] * v[2];
			n[2] = u[0] * v[1] - u[1] * v[0];
			//normalize:
			float n_size = sqrt(pow(n[0], 2) + pow(n[1], 2) + pow(n[2], 2));
			for (int i = 0; i < 3; i++) {
				n[i] = n[i] / n_size;
			}

			//find head x,y:
			//h_z = m_z + t * n_z ---> t = (h_z - m_z) / n_z
			float t = (head[2] - middle[2]) / n[2];
			head[0] = middle[0] + t * n[0];
			head[1] = middle[1] + t * n[1];
		}

		/* Calculates tetrahedron's volume and returns it: */
		float volume() {
			float base_space = triangle_area(left, right, bottom);

			float height = sqrt(pow(head[0] - middle[0], 2) +
								pow(head[1] - middle[1], 2) +
								pow(head[2] - middle[2], 2));

			return base_space * height / 3;
		}

		cv::Vec3f** get_scatter() {
			cv::Vec3f** scatter = new cv::Vec3f*[1];
			scatter[0] = new cv::Vec3f[4];
			scatter[0][0] = left;
			scatter[0][1] = right;
			scatter[0][2] = bottom;
			scatter[0][3] = head;
			return scatter;
		}

		int get_scatter_height() {
			return 1;
		}

		int get_scatter_width() {
			return 4;
		}
	};

	/* Bounding Box for Volume (Reimann Sums in particular) calculations: */
	class BoundingBox {

		float left_x, right_x, top_y, bottom_y;

	public:
		//c'tor:
		BoundingBox() {};

		BoundingBox(float left_x, float right_x, float top_y, float bottom_y) :
			left_x(left_x), right_x(right_x), top_y(top_y), bottom_y(bottom_y) {}

		bool in_bbox(cv::Vec3f& p) {
			if (p[0] < left_x || p[0] > right_x
				|| p[1] < bottom_y || p[1] > top_y) {
				return false;
			}
			return true;
		}

	};

	class Surface {
		BoundingBox bbox;
		int h, w;
		int* cols;
		cv::Vec3f** mat;

	public:
		Surface(const rs2::depth_frame& depth_frame, cv::Vec3f& left, cv::Vec3f& right, cv::Vec3f& mid3);
		float volume();

		cv::Vec3f** get_scatter();
		int get_scatter_height();
		int get_scatter_width();

	private:
		static float area(cv::Vec3f& a, cv::Vec3f& b, cv::Vec3f& c, cv::Vec3f& d);
	};
};

