#include "db_sync.hpp"

std::condition_variable DeepBreathSync::cv_poll_frame;
std::mutex DeepBreathSync::m_poll_frame;
std::atomic<bool> DeepBreathSync::is_poll_frame(false);