#include "synchronized_q3dscatter.hpp"

SynchronizedQ3DScatter::SynchronizedQ3DScatter(const QSurfaceFormat* format, QWindow* parent) : Q3DScatter(format, parent) {
}

std::mutex& SynchronizedQ3DScatter::get_sync_event_mutex()
{
	return sync_event_mutex;
}

bool SynchronizedQ3DScatter::event(QEvent* event) {
	std::unique_lock<std::mutex> lk(sync_event_mutex);
	return Q3DScatter::event(event);
}
