#include "db_sync.hpp"

std::condition_variable DeepBreathSync::poll_frame_cond;
std::mutex DeepBreathSync::m_poll_frame;