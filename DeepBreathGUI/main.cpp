#include <QApplication>

#include <iostream>
#include <thread>

#include "deepbreathgui.h"
#include "db_camera.hpp"
#include "db_sync.hpp"

/*
	Frames polling thread function.
	Activated in the main and runs in the backround, polling for frames only when the boolean is_poll_frame is true.
*/ 
void poll_frames_thread() {

	while (true) {
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



		}
	}
}

int main(int argc, char *argv[])
{
	//initiate frame polling thread:
	std::thread worker(poll_frames_thread);

    QApplication a(argc, argv);
    DeepBreath w;
    w.show();

    return a.exec();
}
