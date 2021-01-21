#pragma once
#include <mutex>
#include <QtDataVisualization/q3dscatter.h>
using namespace QtDataVisualization;

class SynchronizedQ3DScatter : public Q3DScatter {
public:

	static std::mutex sync_event_mutex;

	explicit SynchronizedQ3DScatter(const QSurfaceFormat* format = nullptr, QWindow* parent = nullptr);
protected:
	bool event(QEvent* event);
};