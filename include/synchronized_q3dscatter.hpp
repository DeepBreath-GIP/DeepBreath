#pragma once
#include <mutex>
#include <QtDataVisualization/q3dscatter.h>
using namespace QtDataVisualization;

class SynchronizedQ3DScatter : public Q3DScatter {
public:

	explicit SynchronizedQ3DScatter(const QSurfaceFormat* format = nullptr, QWindow* parent = nullptr);
	std::mutex& get_sync_event_mutex();

protected:
	std::mutex sync_event_mutex;
	bool event(QEvent* event);
};