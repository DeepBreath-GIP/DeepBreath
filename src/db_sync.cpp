#include "db_sync.hpp"

std::condition_variable DeepBreathSync::cv_poll_frame;
std::mutex DeepBreathSync::m_poll_frame;
std::condition_variable DeepBreathSync::cv_end_poll_frame;
std::mutex DeepBreathSync::m_end_poll_frame;
std::atomic<bool> DeepBreathSync::is_poll_frame(false);
std::atomic<bool> DeepBreathSync::is_end_poll_frame(false);
std::atomic<bool> DeepBreathSync::is_active(true);