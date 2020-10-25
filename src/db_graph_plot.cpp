#include "db_graph_plot.hpp"
#include "db_frame_manager.hpp"
#include "db_config.hpp"
#include "db_log.hpp"

#define NUM_OF_LAST_FRAMES 256

/* Forward declarations: */
static void normalize_distances(std::vector<cv::Point2d>* samples);
static long double calc_and_log_frequency_fft(std::vector<cv::Point2d>* samples, std::vector<cv::Point2d>* out_frequencies = NULL);
static void FFT(short int dir, long m, double *x, double *y);

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
	_is_first_plot(true) {

	_graph_widget = graph_widget;

	this->reset();

	_graph_widget->xAxis->setRange(_time_begin, 8, Qt::AlignLeft);


	_graph_widget->yAxis->setRange(0, 200, Qt::AlignLeft);

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
			}
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

}

void DeepBreathGraphPlot::reset() {

	DeepBreathFrameManager& frame_manager = DeepBreathFrameManager::getInstance();

	clock_t current_system_time = clock();
	_time_begin = (current_system_time - frame_manager.manager_start_time) / double(CLOCKS_PER_SEC);
	
	//clear graphs:
	_graph_widget->clearGraphs();
	_is_first_plot = true;
}

void DeepBreathGraphPlot::addData(cv::Point2d& p) {

	DeepBreathConfig& user_cfg = DeepBreathConfig::getInstance();

	switch (user_cfg.mode) {
	case DISTANCES:
		_graph_widget->graph(0)->addData(p.x, p.y);
		break;
		//case FOURIER:
		//	_plotFourier(points);
		//	break;
		//case LOCATION:
		//	_plotLoc(points, i);
		//	break;
		//case VOLUME:
		//	_plotFourier(points);
		//	break;
		//case NOGRAPH:
		//	_plotNoGraph(points);
	}

}

void DeepBreathGraphPlot::update() {
	_graph_widget->replot(QCustomPlot::rpQueuedReplot);
}

void DeepBreathGraphPlot::plot(std::vector<cv::Point2d>& points) {

	DeepBreathConfig& user_cfg = DeepBreathConfig::getInstance();

	switch (user_cfg.mode) {
	case DISTANCES:
		_plotDists(points);
		break;
	//case FOURIER:
	//	_plotFourier(points);
	//	break;
	//case LOCATION:
	//	_plotLoc(points, i);
	//	break;
	//case VOLUME:
	//	_plotFourier(points);
	//	break;
	//case NOGRAPH:
	//	_plotNoGraph(points);
	}

}

void DeepBreathGraphPlot::_plotDists(std::vector<cv::Point2d>& points) {
	long double f;
	normalize_distances(&points);
	f = (calc_and_log_frequency_fft(&points));

	if (points.size() < NUM_OF_LAST_FRAMES * 0.5)
		f = 0;

	long double bpm = f * 60;
	std::string bpm_title("BPM: " + std::to_string(bpm));

	_graph_widget->setWindowTitle(QString::fromStdString(bpm_title));
	for (auto p : points) {
		_graph_widget->graph(0)->addData(p.x, p.y);
	}
	_graph_widget->replot();

}


/* Normalize ditances to create sample with vivid difference: */
static void normalize_distances(std::vector<cv::Point2d>* samples) {
	if (samples->size() < 3) return;
	double max_dist = samples->at(0).y;
	double min_dist = samples->at(0).y;
	for (int i = 1; i < samples->size(); i++) {
		double curr_dist = samples->at(i).y;
		if (curr_dist > max_dist) max_dist = curr_dist;
		if (curr_dist < min_dist) min_dist = curr_dist;
	}
	for (int i = 0; i < samples->size(); i++) {
		double temp_t = samples->at(i).x;
		double temp_d = samples->at(i).y;
		double norm_d = 2 * (temp_d - min_dist) / (max_dist - min_dist) - 1;
		samples->at(i) = cv::Point2d(temp_t, norm_d);
	}
}

/*
* To be used in D mode
* returns most dominant frequency, calculated for average distance in frames received in the last 15 seconds
* the avg distance is calculated only for distances set to true in user_cfg.dists_included
* Logs the result.
*/
static long double calc_and_log_frequency_fft(std::vector<cv::Point2d>* samples, std::vector<cv::Point2d>* out_frequencies) {

	DeepBreathLog& log = DeepBreathLog::getInstance();
	assert(log);

	if (samples->size() < 5) {
		log.log_file << '\n';
		return 0;
	}
	int realSamplesNum = samples->size();	// N - number of samples (frames)
	int realSamplesLog = log2(realSamplesNum);
	// fft requires that the number of samples is a power of 2. add padding if needed
	const int paddedSamplesNum = (realSamplesNum == pow(2, realSamplesLog)) ? realSamplesNum : pow(2, realSamplesLog + 1);
	int dir = 1;
	long m = log2(paddedSamplesNum);
	double* X = new double[paddedSamplesNum];
	double* Y = new double[paddedSamplesNum];
	// insert real samples in first #realSamplesNum slots
	for (int s = 0; s < realSamplesNum; s++) {
		X[s] = samples->at(s).y;
		Y[s] = 0;	// no imaginary part in samples
	}
	// add padding after real samples
	for (int s = realSamplesNum; s < paddedSamplesNum; s++) {
		X[s] = 0;
		Y[s] = 0;
	}

	double t0 = samples->at(0).x;	// t0, t1 - start, end time(seconds)
	double t1 = samples->at(realSamplesNum - 1).x;
	if (t1 == t0) return 0;
	double fps = realSamplesNum / (t1 - t0);	// FPS - frames per second

	FFT(dir, m, X, Y);

	// following comented code return the average of -top- frequencies found by fourier
	/*const int top = 100;
	int top_max_idx[top] = { 0 };
	double min_bpm = 7.0;
	int mini = ceil((min_bpm / 60.0)*((paddedSamplesNum - 2.0) / fps));	// assume BPM >= min_bpm
	for (int j = 0; j < top; j++) {
		int max_idx = 0;
		double max_val = 0;
		for (int i = mini; i < realSamplesNum / 2; i++) { //frequency 0 always most dominant. ignore first coef.
			double val = abs(X[i])*abs(X[i]) + abs(Y[i])*abs(Y[i]);
			if (val > max_val) {
				max_val = val;
				max_idx = i;
			}
		}
		top_max_idx[j] = max_idx;
		X[max_idx] = 0;
	}
	double avg_max_idx = 0;
	for (int i = 0; i < top; i++)  avg_max_idx += top_max_idx[i];
	avg_max_idx /= top;
	long double f = fps / (paddedSamplesNum - 2.0) * top_max_idx[0];
	long double f_avg = fps / (paddedSamplesNum - 2.0) * avg_max_idx;
	*/

	int max_idx = 0;
	double max_val = 0;
	double min_bpm = 5.0;	// TODO: decide minimal BPM
	int mini = ceil((min_bpm / 60.0)*((paddedSamplesNum - 2.0) / fps));	// assume BPM >= min_bpm
	for (int i = 0; i < realSamplesNum / 2; i++) { //frequency 0 always most dominant. ignore first coef.
		double val = abs(X[i])*abs(X[i]) + abs(Y[i])*abs(Y[i]);
		if (out_frequencies != NULL) {
			double frequency = fps / (paddedSamplesNum - 2.0) * i;
			//out_frequencies->push_back(cv::Point2d(frequency, abs(X[i])));
			out_frequencies->push_back(cv::Point2d(frequency, val));
		}
		if (i >= mini && val > max_val) {
			max_val = val;
			max_idx = i;
		}
	}

	delete X;
	delete Y;

	log.log_file << fps << ',';
	log.log_file << realSamplesNum << ',';
	long double f = fps / (paddedSamplesNum - 2.0) * max_idx;
	log.log_file << std::fixed << std::setprecision(6) << f << ',';
	log.log_file << std::fixed << std::setprecision(6) << f * 60.0 << '\n';
	return f;
}

static void FFT(short int dir, long m, double *x, double *y) {
	long n, i, i1, j, k, i2, l, l1, l2;
	double c1, c2, tx, ty, t1, t2, u1, u2, z;
	/* Calculate the number of points */
	n = 1;
	for (i = 0; i < m; i++)
		n *= 2;
	/* Do the bit reversal */
	i2 = n >> 1;
	j = 0;


	for (i = 0; i < n - 1; i++) {
		if (i < j) {
			tx = x[i];
			ty = y[i];
			x[i] = x[j];
			y[i] = y[j];
			x[j] = tx;
			y[j] = ty;
		}
		k = i2;
		while (k <= j) {
			j -= k;
			k >>= 1;
		}
		j += k;
	}

	/* Compute the FFT */
	c1 = -1.0;
	c2 = 0.0;
	l2 = 1;
	for (l = 0; l < m; l++) {
		l1 = l2;
		l2 <<= 1;
		u1 = 1.0;
		u2 = 0.0;
		for (j = 0; j < l1; j++) {
			for (i = j; i < n; i += l2) {
				i1 = i + l1;
				t1 = u1 * x[i1] - u2 * y[i1];
				t2 = u1 * y[i1] + u2 * x[i1];
				x[i1] = x[i] - t1;
				y[i1] = y[i] - t2;
				x[i] += t1;
				y[i] += t2;
			}
			z = u1 * c1 - u2 * c2;
			u2 = u1 * c2 + u2 * c1;
			u1 = z;
		}
		c2 = sqrt((1.0 - c1) / 2.0);
		if (dir == 1)
			c2 = -c2;
		c1 = sqrt((1.0 + c1) / 2.0);
	}

	/* Scaling for forward transform */
	if (dir == 1) {
		for (i = 0; i < n; i++) {
			x[i] /= n;
			y[i] /= n;
		}
	}
}