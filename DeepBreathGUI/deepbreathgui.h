#ifndef DEEPBREATH_H
#define DEEPBREATH_H

#include <QMainWindow>
#include <map>
#include "db_camera.hpp"
#include "CustomOpenGLWidget.hpp"

QT_BEGIN_NAMESPACE
namespace Ui { class DeepBreath; }
QT_END_NAMESPACE

class QDeepBreath : public QMainWindow
{
    Q_OBJECT

public:
    QDeepBreath(QWidget *parent = nullptr);
    ~QDeepBreath();
    void paintEvent(QPaintEvent* event);

	/*
		Render the frames (up to 3: color, depth and IR) to theCustom GL Widgets for the streaming.
		parameters:
		@render_frames - map of frames to render.
		@width - width of the rendered frame.
		@height - height of the rendered frame.
		It is recommended to use width & height of the color frame (bigger) to upscale other frames.
	*/
	void renderStreamWidgets(std::map<int, rs2::frame>& render_frames, const int width, const int height);

private slots:
    void on_left_mid1_checkbox_clicked();

    void on_left_right_checkbox_clicked();

    void on_right_mid1_checkbox_clicked();

    void on_left_mid2_checkbox_clicked();

    void on_mid1_mid2_checkbox_clicked();

    void on_right_mid2_checkbox_clicked();

    void on_right_mid3_checkbox_clicked();

    void on_left_mid3_checkbox_clicked();

    void on_mid1_mid3_checkbox_clicked();

    void on_mid2_mid3_checkbox_clicked();

    void on_start_camera_button_clicked();

    void on_record_button_clicked();

    void on_load_file_button_clicked();

    void on_pause_button_clicked();

    void on_mode_combo_box_currentIndexChanged(int index);

    void on_is_stickers_checkbox_clicked();

private:
    Ui::DeepBreath *ui;

	/* Booleans for click handling: */
    bool is_camera_on;			//camera is on/off
    bool is_recording;			//recording is on/off - activated only when camera is on.
    bool is_run_from_file;		//indicator whether the stream is from file. camera is off when true.
    bool is_pause;				//pause/continue the file stream, active only when file stream is on.

	/* Initialize default settings as they appear in the config file: */
	void initDefaultSelection();

	/* Mark distances selected in config file: */
	void selectDefaultDistances();

	/* Sets transparent backrounds to certain objects: */
    void setTransparentBackroundToText();

	/* Draws the lines on the distances according to their checked state:
		NOTE: Renders only upon paintEvent trigger (See paintEvent) */
    void drawDistancesLines();

	/* Helper for the distances line drawing: */
    void setXYPosOfDistance(int& x1_pos, int& y1_pos, int& x2_pos, int& y2_pos, distances dist);

	/* Enablers/Diablers of ui objects: */
    void enableDistances(bool is_enabled);
    void enableLocations(bool is_enabled);
    void enableMenu(bool is_enabled);
};
#endif // DEEPBREATH_H
