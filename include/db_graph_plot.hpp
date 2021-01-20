#pragma once

#include <opencv2/opencv.hpp>

#include "qcustomplot.hpp"

class DeepBreathGraphPlot {

public:

	static void createInstance(QCustomPlot* graph_widget);

	static DeepBreathGraphPlot& getInstance();

	void reset();

	void addData(cv::Point2d& p, int s = 0);

	void update();

	void clear();

protected:

	//ctor:
	DeepBreathGraphPlot(QCustomPlot* graph_widget);

	static DeepBreathGraphPlot* _graph_plot;

private:

	QCustomPlot* _graph_widget;
	clock_t _time_begin;
	bool _is_first_plot;
	float _min_x;
	float _max_x;
	float _min_y;
	float _max_y;

	float _min_vol; //helper for volume difference plotting

};
