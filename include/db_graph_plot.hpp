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

	void plot(std::vector<cv::Point2d>& points);

protected:

	//ctor:
	DeepBreathGraphPlot(QCustomPlot* graph_widget);

	static DeepBreathGraphPlot* _graph_plot;

private:

	//void _plotFourier(std::vector<cv::Point2d>& points);
	void _plotDists(std::vector<cv::Point2d>& points);
	//void _plotLoc(std::vector<cv::Point2d>& points, int i);
	//void _plotVolume(std::vector<cv::Point2d>& points);
	//void _plotNoGraph(std::vector<cv::Point2d>& points);

	QCustomPlot* _graph_widget;
	clock_t _time_begin;
	bool _is_first_plot;

};
