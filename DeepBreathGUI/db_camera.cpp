/* DeepBreath Files */
#include "db_camera.hpp"

DeepBreathCamera* DeepBreathCamera::_camera = nullptr;

const DeepBreathCamera & DeepBreathCamera::getInstance()
{
	if (_camera == nullptr) {
		_camera = new DeepBreathCamera();
	}

	return *_camera;
}

DeepBreathCamera::DeepBreathCamera() :
	depth_align(RS2_STREAM_DEPTH),
	color_align(RS2_STREAM_COLOR)
{

}