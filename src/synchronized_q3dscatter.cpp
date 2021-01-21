#include "synchronized_q3dscatter.hpp"

std::mutex SynchronizedQ3DScatter::sync_event_mutex;

SynchronizedQ3DScatter::SynchronizedQ3DScatter(const QSurfaceFormat* format, QWindow* parent) : Q3DScatter(format, parent) {
}

bool SynchronizedQ3DScatter::event(QEvent* event) {
	std::unique_lock<std::mutex> lk(SynchronizedQ3DScatter::sync_event_mutex);
	return Q3DScatter::event(event);
}
