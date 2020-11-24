#include "db_graph_plot.hpp"
#include "db_frame_manager.hpp"
#include "db_config.hpp"
#include "db_log.hpp"

#define NUM_OF_LAST_FRAMES 256

/* Forward declarations: */


/* ** */

DeepBreathGraphPlot* DeepBreathGraphPlot::_graph_plot = nullptr;

void DeepBreathGraphPlot::createInstance(QCustomPlot* graph_widget) {
	if (_graph_plot == nullptr) {
		_graph_plot = new DeepBreathGraphPlot(graph_widget);

	}
	else {
		_graph_plot->reset();
	}
}

DeepBreathGraphPlot & DeepBreathGraphPlot::getInstance()
{
	return *_graph_plot;
}


DeepBreathGraphPlot::DeepBreathGraphPlot(QCustomPlot* graph_widget) :
	_is_first_plot(true),
	_min_x(0),
	_max_x(0),
	_min_y(0),
	_max_y(0),
	_min_vol(0) {

	_graph_widget = graph_widget;

	this->reset();

	_graph_widget->xAxis->setRange(_time_begin, 8, Qt::AlignLeft);

	_graph_widget->yAxis->setRange(0, 200, Qt::AlignLeft);

	_graph_widget->setInteraction(QCP::iRangeDrag, true);
	_graph_widget->setInteraction(QCP::iRangeZoom, true);

	DeepBreathConfig& user_cfg = DeepBreathConfig::getInstance();

	int i = 0;
	QPen pen(QColor(qSin(i*0.3) * 100 + 100, qSin(i*0.6 + 0.7) * 100 + 100, qSin(i*0.4 + 0.6) * 100 + 100));

	switch (user_cfg.mode) {
	case LOCATION:
		//Add graphs as the number of locations to show:
		for (auto loc : user_cfg.stickers_included) {
			if (loc.second == true) {
				_graph_widget->addGraph(); // blue line
				_graph_widget->graph(i)->setPen(pen);
				i++;
			}
			pen.setColor(QColor(qSin(i*0.3) * 100 + 100, qSin(i*0.6 + 0.7) * 100 + 100, qSin(i*0.4 + 0.6) * 100 + 100));
		}
		break;
	default:
		//just one graph:
		_graph_widget->addGraph(); // blue line
		_graph_widget->graph(0)->setPen(QPen(QColor(40, 110, 255)));
		break;
	}

}

void DeepBreathGraphPlot::reset() {

	DeepBreathFrameManager& frame_manager = DeepBreathFrameManager::getInstance();

	clock_t current_system_time = clock();
	_time_begin = (current_system_time - frame_manager.manager_start_time) / double(CLOCKS_PER_SEC);
	
	//clear graphs:
	_graph_widget->clearGraphs();

	//and set new brushes for new graphs:
	DeepBreathConfig& user_cfg = DeepBreathConfig::getInstance();

	int i = 0;
	QPen pen(QColor(qSin(i*0.3) * 100 + 100, qSin(i*0.6 + 0.7) * 100 + 100, qSin(i*0.4 + 0.6) * 100 + 100));

	switch (user_cfg.mode) {
	case LOCATION:
		//Add graphs for all locations (in the beginning all are empty):
		for (auto loc : user_cfg.stickers_included) {
			_graph_widget->addGraph();
			_graph_widget->graph(i)->setPen(pen);
			//update pen for next iteration:
			i++;
			pen.setColor(QColor(qSin(i*0.3) * 100 + 100, qSin(i*0.6 + 0.7) * 100 + 100, qSin(i*0.4 + 0.6) * 100 + 100));
		}
		break;
	default:
		//just one graph:
		_graph_widget->addGraph(); // blue line
		_graph_widget->graph(0)->setPen(QPen(QColor(40, 110, 255)));
		break;
	}

	_is_first_plot = true;
	_min_x = 0;
	_max_x = 0;
	_min_y = 0;
	_max_y = 0;
	_min_vol = 0;
}

void DeepBreathGraphPlot::addData(cv::Point2d& p, int s) {

	DeepBreathConfig& user_cfg = DeepBreathConfig::getInstance();

	if (user_cfg.mode == VOLUME) {
		if (_min_vol == 0) {
			//don't plot the first volume that arrives:
			_min_vol = p.y;
			return;
		}

		//set y ranges according to difference (and not p.y):
		if (p.y - _min_vol > _max_y) {
			_max_y = p.y - _min_vol;
			_graph_widget->yAxis->setRange(_min_y, _max_y);
		}
		if (p.y - _min_vol < _min_y || _min_y == 0) {
			_min_y = p.y - _min_vol;
			_graph_widget->yAxis->setRange(_min_y, _max_y);
		}

	}
	else {

		if (p.y > _max_y) {
			_max_y = p.y;
			_graph_widget->yAxis->setRange(_min_y, _max_y);
		}
		if (p.y < _min_y || _min_y == 0) {
			_min_y = p.y;
			_graph_widget->yAxis->setRange(_min_y, _max_y);
		}

	}

	if (p.x > _max_x) {
		_max_x = p.x;
	}
	if (p.x < _min_x) {
		_min_x = p.x;
	}

	//set graph running to right:
	if (p.x > 8) {
		_graph_widget->xAxis->setRange(p.x, 8, Qt::AlignRight);
	}

	switch (user_cfg.mode) {
	case LOCATION:
		_graph_widget->graph(s)->addData(p.x, p.y);
		break;
	case VOLUME:
		_graph_widget->graph(0)->addData(p.x, p.y - _min_vol);
		if (p.y < _min_vol) {
			_min_vol = p.y;
		}
		break;
	case NOGRAPH:
		//No data to add, do nothing.
		break;
	default: //distances, fourier - the same:
		_graph_widget->graph(0)->addData(p.x, p.y);
		break;
	}

}

void DeepBreathGraphPlot::update() {
	_graph_widget->replot(QCustomPlot::rpQueuedReplot);
}
