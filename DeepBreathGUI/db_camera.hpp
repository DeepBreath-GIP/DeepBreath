#pragma once

#include <librealsense2/rs.hpp>
#include <chrono>

/*
	Realsense camera objects used by app.
	Singleton object.
	@ colorizer: helper to colorize depth images.
	@ pipe: pipeline to easily configure and start the camera.
	@ cfg:
	@ depth_align, color_align: define two align objects.
		one will be used to align to depth viewport and the other to color.
		creating align object is an expensive operation, should only be done once - in the constructor
	@ fs: frameset object updated by polling for frames.
*/
class DeepBreathCamera {

public:

	static const DeepBreathCamera& getInstance();

	rs2::colorizer colorizer;
	rs2::pipeline pipe;
	rs2::config cfg;
	rs2::align depth_align;
	rs2::align color_align;
	rs2::frameset fs;

protected:

	DeepBreathCamera();

	static DeepBreathCamera* _camera;

};