#pragma once

#include <condition_variable>
#include <mutex>
#include <atomic>

/*
	Synchronization object for frame polling.
	@ poll_frame_cond - condition variable used for thread awakening
	@ m_poll_frame - mutex to perform the modification while the lock is held
	@ is_poll_frame - boolean (atomic) to indicate when to stop polling
*/
class DeepBreathSync {

public:

	static std::condition_variable cv_poll_frame;
	static std::mutex m_poll_frame;
	static std::condition_variable cv_end_poll_frame;
	static std::mutex m_end_poll_frame;
	static std::atomic<bool> is_poll_frame;
	static std::atomic<bool> is_active;
};