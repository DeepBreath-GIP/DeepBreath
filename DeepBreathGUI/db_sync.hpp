#pragma once

#include <condition_variable>
#include <mutex>

/*
	Synchronization object for frame polling.
	@ poll_frame_cond - condition variable used for thread awakening
	@ m_poll_frame - mutex to perform the modification while the lock is held
*/
class DeepBreathSync {

public:

	static std::condition_variable poll_frame_cond;
	static std::mutex m_poll_frame;

};