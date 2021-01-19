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

	rs2::spatial_filter spat;
	spat.set_option(RS2_OPTION_HOLES_FILL, 5);
	while (DeepBreathSync::is_active) {
		// Wait until main() sends data
		std::unique_lock<std::mutex> lk(DeepBreathSync::m_poll_frame);
		DeepBreathSync::cv_poll_frame.wait(lk);
		lk.unlock();

		try {
			// Do polling
			while (DeepBreathSync::is_poll_frame) {
				//poll and process
				DeepBreathCamera camera = DeepBreathCamera::getInstance();
				camera.fs = camera.pipe.wait_for_frames(1000);

				// align all frames to color viewport
				camera.fs = camera.color_align.process(camera.fs);
				// with the aligned frameset we proceed as usual
				auto depth = camera.fs.get_depth_frame();

				//auto pc_depth = camera.fs.get_depth_frame();
				auto color = camera.fs.get_color_frame();

				//collect all frames:
				//using a map as in rs-multicam to allow future changes in number of cameras displayed.
				std::map<int, rs2::frame> render_frames;

				//process frame with the frame manager:
				DeepBreathFrameManager& frame_manager = DeepBreathFrameManager::getInstance();
				frame_manager.process_frame(color, spat.process(depth));

				// convert the newly-arrived frames to render-firendly format
				//for (const auto& frame : fs) //iterate over all available frames. (commented out to ignore IR emitter frames.)
				//{
				render_frames[color.get_profile().unique_id()] = camera.colorizer.process(color);
				render_frames[depth.get_profile().unique_id()] = camera.colorizer.process(depth);
				//}

				//render frames to the gl objects:
				db_ref->renderStreamWidgets(render_frames, color.get_width(), color.get_height());

				//get last frames data and calculate breath rate:
				long double bpm = frame_manager.calculate_breath_rate();
				db_ref->updateBPM(bpm);

				long double fps = frame_manager.get_fps();
				db_ref->updateFPS(fps);

				if (!frame_manager.get_is_last_frame_dumped()) {
					frame_manager.add_last_data_to_graph();
					frame_manager.log_last_frame_data();
					frame_manager.log_breathing_data();
				}
			}
		}
		catch (rs2::error e) {
			if (e.get_type() == RS2_EXCEPTION_TYPE_COUNT) {
				// Trigger stop 
				db_ref->stop_file();
			}
			else {
				throw e;
			}
		}

		// Notify the end of the polling
		DeepBreathSync::is_end_poll_frame = true;
		DeepBreathSync::cv_end_poll_frame.notify_one();
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
