#ifndef DEEPBREATH_H
#define DEEPBREATH_H

#include <QMainWindow>
#include <map>
#include <opencv2/opencv.hpp>
#include "custom_opengl_widget.hpp"
#include "db_config.hpp"
#include "db_camera.hpp"

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
	void renderStreamWidgets(std::vector<rs2::frame>& render_frames, const int width, const int height);

    void renderScatterWidget();

	/*
		Update BPM in ui:
	*/
	void updateBPM(long double bpm);

    /*
    Update FPS in ui:
    */
	void updateFPS(long double fps);

    void stop_file();

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

    void on_left_loc_checkbox_clicked();

    void on_mid1_loc_checkbox_clicked();

    void on_right_loc_checkbox_clicked();

    void on_mid2_loc_checkbox_clicked();

    void on_mid3_loc_checkbox_clicked();

	void on_dimension_2d_radio_button_clicked();

	void on_dimension_3d_radio_button_clicked();

	void on_num_markers_combo_box_currentIndexChanged(int index);

	void on_volume_type_combo_box_currentIndexChanged(int index);

private:
    Ui::DeepBreath *ui;

	/* Booleans for click handling: */
    bool is_camera_on;			//camera is on/off
    bool is_recording;			//recording is on/off - activated only when camera is on.
    bool is_run_from_file;		//indicator whether the stream is from file. camera is off when true.
    bool is_pause;				//pause/continue the file stream, active only when file stream is on.

	/* Initialize default settings as they appear in the config file: */
	void initDefaultSelection();

	/* Mark distances selected in config: */
	void setConfigDistances();

	/* Mark locations selected in config: */
	void setConfigLocations();

	/* Select the dimension in config: */
	void setConfigDimension();

	/* Draws the lines on the distances according to their checked state:
		NOTE: Renders only upon paintEvent trigger (See paintEvent) */
    void drawDistancesLines();

	/* Helper for the distances line drawing: */
    void setXYPosOfDistance(int& x1_pos, int& y1_pos, int& x2_pos, int& y2_pos, distances dist);

	/* Enablers/Diasblers of ui objects: */
    void enableDistances(bool is_enabled);
    void enableLocations(bool is_enabled);
    void enableMenu(bool is_enabled);
    void toggleMenuOnStream(bool is_enabled);

    void clearStreamingWidgets();
    void clearScatterWidget();

    void stop_frame_polling();
    void start_frame_polling();
};
#endif // DEEPBREATH_H
