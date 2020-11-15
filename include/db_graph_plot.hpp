#pragma once

#include <opencv2/opencv.hpp>

#include "qcustomplot.h"

class DeepBreathGraphPlot {

public:

	static void createInstance(QCustomPlot* graph_widget);

	static DeepBreathGraphPlot& getInstance();

	void reset();

	void addData(cv::Point2d& p);

	void update();

protected:

	//ctor:
	DeepBreathGraphPlot(QCustomPlot* graph_widget);

	static DeepBreathGraphPlot* _graph_plot;

private:

	QCustomPlot* _graph_widget;
	clock_t _time_begin;
	bool _is_first_plot;

};
