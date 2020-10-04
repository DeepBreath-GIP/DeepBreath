#include <QApplication>

#include <iostream>
#include <thread>

#include "deepbreathgui.h"
#include "db_sync.hpp"

void poll_frames_thread() {

	std::cout << "Thread Started" << std::endl;

	while (true) {
		// Wait until main() sends data
		std::unique_lock<std::mutex> lk(DeepBreathSync::m_poll_frame);
		DeepBreathSync::cv_poll_frame.wait(lk);
		lk.unlock();

		// Do polling
		while (DeepBreathSync::is_poll_frame) {
			std::cout << "Polling frame" << std::endl;
		}
	}

}

int main(int argc, char *argv[])
{

	std::thread worker(poll_frames_thread);

    QApplication a(argc, argv);
    DeepBreath w;
    w.show();

    return a.exec();
}
