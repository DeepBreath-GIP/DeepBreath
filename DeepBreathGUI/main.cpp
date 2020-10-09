#include <QApplication>
#include <iostream>
#include <thread>

#include "deepbreathgui.h"
#include "db_camera.hpp"
#include "db_sync.hpp"
#include "db_frame_manager.hpp"

/*
	Frames polling thread function.
	Activated in the main and runs in the backround, polling for frames only when the boolean is_poll_frame is true.
*/ 
void poll_frames_thread(QDeepBreath* db_ref) {

	while (DeepBreathSync::is_active) {
		// Wait until main() sends data
		std::unique_lock<std::mutex> lk(DeepBreathSync::m_poll_frame);
		DeepBreathSync::cv_poll_frame.wait(lk);
		lk.unlock();

		// Do polling
		while (DeepBreathSync::is_poll_frame) {
			//poll and process
			DeepBreathCamera camera = DeepBreathCamera::getInstance();
			camera.fs = camera.pipe.wait_for_frames();

			// align all frames to color viewport
			camera.fs = camera.color_align.process(camera.fs);
			// with the aligned frameset we proceed as usual
			auto depth = camera.fs.get_depth_frame();
			auto color = camera.fs.get_color_frame();
			auto colorized_depth = camera.colorizer.colorize(depth);

			//collect all frames:
			//using a map as in rs-multicam to allow future changes in number of cameras displayed.
			std::map<int, rs2::frame> render_frames;

			//TODO:
			DeepBreathFrameManager& frame_manager = DeepBreathFrameManager::getInstance();
			//frame_manager.process_frame(color, depth);

			// convert the newly-arrived frames to render-firendly format
			//for (const auto& frame : fs) //iterate over all available frames. (commented out to ignore IR emitter frames.)
			//{
			render_frames[color.get_profile().unique_id()] = camera.colorizer.process(color);
			render_frames[depth.get_profile().unique_id()] = camera.colorizer.process(depth);
			//}

			db_ref->renderStreamWidgets(render_frames, color.get_width(), color.get_height());

		}
	}
}

int main(int argc, char *argv[])
{

    QApplication a(argc, argv);
    QDeepBreath w;

	//first initiation:
	DeepBreathCamera& camera = DeepBreathCamera::getInstance();
	DeepBreathFrameManager& frame_manager = DeepBreathFrameManager::getInstance();

	//initiate frame polling thread:
	std::thread worker(poll_frames_thread, &w);

    w.show();

	a.exec();

	// Disable polling thread
	DeepBreathSync::is_poll_frame = false;
	DeepBreathSync::is_active = false;
	DeepBreathSync::cv_poll_frame.notify_one();

	worker.join();

	return 0;
}
