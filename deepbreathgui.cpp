
/* Deep Breath App */

/* By Nili Furman (Based on Realtime Breathing), supervised by Alon Zrivin and Yaron Honen from GIP Lab
   of the Technion - Israel Institute of Technology
   For the use of Ichilov hospital in Israel. */

#include <sstream>

#include "deepbreathgui.h"
#include "ui_deepbreathgui.h"
#include <Qt>
#include <QPainter>
#include <QPen>
#include <QLine>
#include <QMessageBox>
#include <chrono>
using namespace std::chrono_literals;

/* DeepBreath Files */
#include "db_camera.hpp"
#include "db_sync.hpp"
#include "db_frame_manager.hpp"
#include "db_log.hpp"
#include "db_graph_plot.hpp"

#define FILE_ON_REPEAT false

// copied os.h because project does comile when including it (seems to be due to double inclusion of rendering.h)
// *****	START of os.h copy	*****

namespace rs2
{
	// Wrapper for cross-platform dialog control
	enum file_dialog_mode {
		open_file = (1 << 0),
		save_file = (1 << 1),
	};
	const char* file_dialog_open(file_dialog_mode flags, const char* filters, const char* default_path, const char* default_name);
}

// *****	END of os.h copy	*****

//NOTE FOR SELF:
//To Debug on release mode:
//https://docs.microsoft.com/en-us/cpp/build/how-to-debug-a-release-build?view=vs-2019

QDeepBreath::QDeepBreath(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::DeepBreath)
    , is_camera_on(false)
    , is_recording(false)
    , is_run_from_file(false)
    , is_pause(false)
{
    ui->setupUi(this);
    ui->record_button->setVisible(false);
    ui->pause_button->setVisible(false);

	initDefaultSelection();
}

QDeepBreath::~QDeepBreath()
{
    delete ui;
}

void QDeepBreath::paintEvent(QPaintEvent* event) {
    drawDistancesLines();
}

void QDeepBreath::renderStreamWidgets(std::map<int, rs2::frame>& render_frames, int width, int height) {

	CustomOpenGLWidget** streams_widgets = new CustomOpenGLWidget*[3];
	streams_widgets[0] = (CustomOpenGLWidget*)(ui->color_stream_widget);
	streams_widgets[1] = (CustomOpenGLWidget*)(ui->depth_stream_widget);
	streams_widgets[2] = (CustomOpenGLWidget*)(ui->volume_stream_widget);

	int i = 0;

	for (auto& frame : render_frames)
	{
		if (i >= 2)	//render only color and depth from map
			break;
		const void * frame_data = frame.second.get_data();

		// Copy frame_data to avoid race condition on freeing the frame while rendering the widget.
		uchar * copy_frame_data = new uchar[width * height * 3];
		memcpy(copy_frame_data, frame_data, sizeof(uchar) * width * height * 3);

		QImage frame_data_img(copy_frame_data, width, height, QImage::Format_RGB888);
		streams_widgets[i]->display(frame_data_img);
		i++;
	}
}

void QDeepBreath::updateBPM(long double bpm) {

	std::stringstream ss;
	ss << std::fixed << std::setprecision(2) << bpm;
	std::string bpm_str = std::string(ss.str());
	const QString bpm_q = QString::fromStdString(bpm_str);
	ui->bpm_value->setText(bpm_q);
}

void QDeepBreath::updateFPS(long double fps) {

	std::stringstream ss;
	ss << std::fixed << std::setprecision(2) << fps;
	std::string fps_str = std::string(ss.str());
	const QString fps_q = QString::fromStdString(fps_str);
	ui->fps_value->setText(fps_q);
}

void QDeepBreath::stop_file() {
	ui->load_file_button->clicked();
}

/*	Initiates the UI to default selection as configured in the config file. */
void QDeepBreath::initDefaultSelection() {
	//create and get instance of the config.
	//TODO: remove config_err
	std::string config_err;
	DeepBreathConfig::createInstance(CONFIG_FILEPATH, &config_err);
	DeepBreathConfig& user_cfg = DeepBreathConfig::getInstance();
	assert(user_cfg != nullptr);

	//Set dimension:
	switch (user_cfg.dimension) {
	case D2:
		ui->dimension_2d_radio_button->setChecked(true);
		ui->dimension_3d_radio_button->setChecked(false);
		break;
	case D3:
		ui->dimension_2d_radio_button->setChecked(false);
		ui->dimension_3d_radio_button->setChecked(true);
		break;
	}

	//set mode
	//and set marks according to it:
	int mode_index = -1; //initialize to invalid index by default
	switch (user_cfg.mode) {
	case DISTANCES:
		mode_index = ui->mode_combo_box->findText("Distances");
		setConfigDistances();
		ui->volume_type_combo_box->setVisible(false);
		break;
	case LOCATION:
		mode_index = ui->mode_combo_box->findText("Locations");
		setConfigLocations();
		ui->volume_type_combo_box->setVisible(false);
		break;
	case FOURIER:
		mode_index = ui->mode_combo_box->findText("Fourier");
		ui->volume_type_combo_box->setVisible(false);
		break;
	case VOLUME:
		mode_index = ui->mode_combo_box->findText("Volume");
		ui->volume_type_combo_box->setVisible(true);
		break;
	case NOGRAPH:
		mode_index = ui->mode_combo_box->findText("No Graph");
		ui->volume_type_combo_box->setVisible(false);
		break;
	}
	ui->mode_combo_box->setCurrentIndex(mode_index);

	int volume_type_index = -1; //initialize to invalid index by default
	switch (user_cfg.volume_type) {
	case TETRAHEDRON:
		volume_type_index = ui->volume_type_combo_box->findText("Tetrahedron");
		break;
	case REIMANN:
		volume_type_index = ui->volume_type_combo_box->findText("Reimann Sums");
		break;
	}
	ui->volume_type_combo_box->setCurrentIndex(volume_type_index);

	//set number of markers:
	int num_markers_index = -1; //initialize to invalid index by default
	switch (user_cfg.num_of_stickers) {
	case 3:
		num_markers_index = ui->num_markers_combo_box->findText("3");
		break;
	case 4:
		num_markers_index = ui->num_markers_combo_box->findText("4");
		break;
	case 5:
		num_markers_index = ui->num_markers_combo_box->findText("5");
		break;
	default: //in case it's anything else - ignore the value and set nothing.
		//Will be considered as 0.
		break;
	}
	ui->num_markers_combo_box->setCurrentIndex(num_markers_index);

	//set stickers or image recognition:
	if (user_cfg.is_stickers) {
		ui->is_stickers_checkbox->click(); //initialized unchecked, so triggering a click will check it

		//get sticker color:
		switch (user_cfg.color) {
		case YELLOW:
			ui->y_color_radio_button->click();
			break;
		case BLUE:
			ui->b_color_radio_button->click();
			break;
		case GREEN:
			ui->g_color_radio_button->click();
			break;
		}
	}

}

void QDeepBreath::setConfigDistances() {

	DeepBreathConfig& user_cfg = DeepBreathConfig::getInstance();

	ui->left_mid1_checkbox->setChecked(user_cfg.dists_included[distances::left_mid1]);
	ui->left_mid2_checkbox->setChecked(user_cfg.dists_included[distances::left_mid2]);
	ui->left_mid3_checkbox->setChecked(user_cfg.dists_included[distances::left_mid3]);
	ui->right_mid1_checkbox->setChecked(user_cfg.dists_included[distances::right_mid1]);
	ui->right_mid2_checkbox->setChecked(user_cfg.dists_included[distances::right_mid2]);
	ui->right_mid3_checkbox->setChecked(user_cfg.dists_included[distances::right_mid3]);
	ui->mid1_mid2_checkbox->setChecked(user_cfg.dists_included[distances::mid1_mid2]);
	ui->mid1_mid3_checkbox->setChecked(user_cfg.dists_included[distances::mid1_mid3]);
	ui->mid2_mid3_checkbox->setChecked(user_cfg.dists_included[distances::mid2_mid3]);
	ui->left_right_checkbox->setChecked(user_cfg.dists_included[distances::left_right]);

	if (user_cfg.num_of_stickers == 3) {
		ui->left_mid1_checkbox->setChecked(false);
		ui->left_mid2_checkbox->setChecked(false);
		ui->right_mid1_checkbox->setChecked(false);
		ui->right_mid2_checkbox->setChecked(false);
		ui->mid1_mid2_checkbox->setChecked(false);
		ui->mid1_mid3_checkbox->setChecked(false);
		ui->mid2_mid3_checkbox->setChecked(false);

		ui->left_mid1_checkbox->setEnabled(false);
		ui->left_mid2_checkbox->setEnabled(false);
		ui->right_mid1_checkbox->setEnabled(false);
		ui->right_mid2_checkbox->setEnabled(false);
		ui->mid1_mid2_checkbox->setEnabled(false);
		ui->mid1_mid3_checkbox->setEnabled(false);
		ui->mid2_mid3_checkbox->setEnabled(false);
	}
	else if (user_cfg.num_of_stickers == 4) {
		ui->left_mid1_checkbox->setChecked(false);
		ui->right_mid1_checkbox->setChecked(false);
		ui->mid1_mid2_checkbox->setChecked(false);
		ui->mid1_mid3_checkbox->setChecked(false);

		ui->left_mid1_checkbox->setEnabled(false);
		ui->right_mid1_checkbox->setEnabled(false);
		ui->mid1_mid2_checkbox->setEnabled(false);
		ui->mid1_mid3_checkbox->setEnabled(false);
	}

}

void QDeepBreath::setConfigLocations() {

	DeepBreathConfig& user_cfg = DeepBreathConfig::getInstance();

	ui->left_loc_checkbox->setChecked(user_cfg.stickers_included[stickers::left]);
	ui->right_loc_checkbox->setChecked(user_cfg.stickers_included[stickers::right]);
	ui->mid1_loc_checkbox->setChecked(user_cfg.stickers_included[stickers::mid1]);
	ui->mid2_loc_checkbox->setChecked(user_cfg.stickers_included[stickers::mid2]);
	ui->mid3_loc_checkbox->setChecked(user_cfg.stickers_included[stickers::mid3]);

	if (user_cfg.num_of_stickers == 3) {
		ui->mid1_loc_checkbox->setChecked(false);
		ui->mid2_loc_checkbox->setChecked(false);

		ui->mid1_loc_checkbox->setEnabled(false);
		ui->mid2_loc_checkbox->setEnabled(false);
	}
	else if (user_cfg.num_of_stickers == 4) {
		ui->mid1_loc_checkbox->setChecked(false);

		ui->mid1_loc_checkbox->setEnabled(false);
	}

}

void QDeepBreath::setConfigDimension() {

	DeepBreathConfig& user_cfg = DeepBreathConfig::getInstance();

	if (user_cfg.dimension == dimension::D2) {
		ui->dimension_2d_radio_button->click();
	}
	else {
		ui->dimension_3d_radio_button->click();
	}

}

/* Draw Lines of distances to choose */
void QDeepBreath::drawDistancesLines() {
    QPainter painter(this);

    qreal gray_width(2);
    QPen gray_pen = QPen(Qt::gray, gray_width);

    qreal color_width(3);
    QPen color_pen = QPen(Qt::blue, color_width);

    qreal eraser_width(3);
    QPen eraser_pen = QPen(Qt::transparent, eraser_width);
    painter.setPen(eraser_pen);

    int x1_pos = 0;
    int y1_pos = 0;
    int x2_pos = 0;
    int y2_pos = 0;

    setXYPosOfDistance(x1_pos, y1_pos, x2_pos, y2_pos, left_mid1);
    QLine left_mid1(x1_pos, y1_pos, x2_pos, y2_pos);

    setXYPosOfDistance(x1_pos, y1_pos, x2_pos, y2_pos, right_mid1);
    QLine right_mid1(x1_pos, y1_pos, x2_pos, y2_pos);

    setXYPosOfDistance(x1_pos, y1_pos, x2_pos, y2_pos, left_mid2);
    QLine left_mid2(x1_pos, y1_pos, x2_pos, y2_pos);

    setXYPosOfDistance(x1_pos, y1_pos, x2_pos, y2_pos, right_mid2);
    QLine right_mid2(x1_pos, y1_pos, x2_pos, y2_pos);

    setXYPosOfDistance(x1_pos, y1_pos, x2_pos, y2_pos, left_mid3);
    QLine left_mid3(x1_pos, y1_pos, x2_pos, y2_pos);

    setXYPosOfDistance(x1_pos, y1_pos, x2_pos, y2_pos, right_mid3);
    QLine right_mid3(x1_pos, y1_pos, x2_pos, y2_pos);

    setXYPosOfDistance(x1_pos, y1_pos, x2_pos, y2_pos, left_right);
    QLine left_right(x1_pos, y1_pos, x2_pos, y2_pos);

    setXYPosOfDistance(x1_pos, y1_pos, x2_pos, y2_pos, mid1_mid2);
    QLine mid1_mid2(x1_pos, y1_pos, x2_pos, y2_pos);

    setXYPosOfDistance(x1_pos, y1_pos, x2_pos, y2_pos, mid2_mid3);
    QLine mid2_mid3(x1_pos, y1_pos, x2_pos, y2_pos);

    setXYPosOfDistance(x1_pos, y1_pos, x2_pos, y2_pos, mid1_mid3);
    QLine mid1_mid3(x1_pos, y1_pos, x2_pos, y2_pos);

    //delete old lines:
    painter.setPen(eraser_pen);
    painter.drawLine(left_mid1);
    painter.drawLine(right_mid1);
    painter.drawLine(left_mid2);
    painter.drawLine(right_mid2);
    painter.drawLine(left_mid3);
    painter.drawLine(right_mid3);
    painter.drawLine(left_right);
    painter.drawLine(mid1_mid2);
    painter.drawLine(mid2_mid3);
    painter.drawLine(mid1_mid3);

    //draw new lines:

    if(ui->left_mid1_checkbox->isChecked()) {
        painter.setPen(color_pen);
        painter.drawLine(left_mid1);
    }
    else {
        painter.setPen(gray_pen);
        painter.drawLine(left_mid1);
    }

    if(ui->right_mid1_checkbox->isChecked()) {
        painter.setPen(color_pen);
        painter.drawLine(right_mid1);
    }
    else {
        painter.setPen(gray_pen);
        painter.drawLine(right_mid1);
    }

    if(ui->left_mid2_checkbox->isChecked()) {
        painter.setPen(color_pen);
        painter.drawLine(left_mid2);
    }
    else {
        painter.setPen(gray_pen);
        painter.drawLine(left_mid2);
    }

    if(ui->right_mid2_checkbox->isChecked()) {
        painter.setPen(color_pen);
        painter.drawLine(right_mid2);
    }
    else {
        painter.setPen(gray_pen);
        painter.drawLine(right_mid2);
    }

    if(ui->left_mid3_checkbox->isChecked()) {
        painter.setPen(color_pen);
        painter.drawLine(left_mid3);
    }
    else {
        painter.setPen(gray_pen);
        painter.drawLine(left_mid3);
    }

    if(ui->right_mid3_checkbox->isChecked()) {
        painter.setPen(color_pen);
        painter.drawLine(right_mid3);
    }
    else {
        painter.setPen(gray_pen);
        painter.drawLine(right_mid3);
    }

    if(ui->left_right_checkbox->isChecked()) {
        painter.setPen(color_pen);
        painter.drawLine(left_right);
    }
    else {
        painter.setPen(gray_pen);
        painter.drawLine(left_right);
    }

    if(ui->mid1_mid2_checkbox->isChecked()) {
        painter.setPen(color_pen);
        painter.drawLine(mid1_mid2);
    }
    else {
        painter.setPen(gray_pen);
        painter.drawLine(mid1_mid2);
    }

    if(ui->mid2_mid3_checkbox->isChecked()) {
        painter.setPen(color_pen);
        painter.drawLine(mid2_mid3);
    }
    else {
        painter.setPen(gray_pen);
        painter.drawLine(mid2_mid3);
    }

    if(ui->mid1_mid3_checkbox->isChecked()) {
        painter.setPen(color_pen);
        painter.drawLine(mid1_mid3);
    }
    else {
        painter.setPen(gray_pen);
        painter.drawLine(mid1_mid3);
    }

}

void QDeepBreath::setXYPosOfDistance(int& x1_pos, int& y1_pos, int& x2_pos, int& y2_pos,
                                           distances dist) {
    switch (dist) {
        case left_right:
            x1_pos = ui->left_text->x() + ui->left_text->width() / 2;
            y1_pos = ui->left_text->y();
            x2_pos = ui->right_text->x() + ui->right_text->width() / 2;
            y2_pos = ui->right_text->y();
            break;
        case left_mid1:
            x1_pos = ui->left_text->x() + ui->left_text->width();
            y1_pos = ui->left_text->y() + ui->left_text->height() / 2;
            x2_pos = ui->mid1_text->x();
            y2_pos = ui->mid1_text->y() + ui->mid1_text->height() / 2;
            break;
        case left_mid2:
            x1_pos = ui->left_text->x() + ui->left_text->width();
            y1_pos = ui->left_text->y() + ui->left_text->height();
            x2_pos = ui->mid2_text->x();
            y2_pos = ui->mid2_text->y() + ui->mid1_text->height() / 2;
            break;
        case left_mid3:
            x1_pos = ui->left_text->x() + ui->left_text->width() / 2;
            y1_pos = ui->left_text->y() + ui->left_text->height();
            x2_pos = ui->mid3_text->x();
            y2_pos = ui->mid3_text->y() + ui->mid3_text->height() / 2;
            break;
        case right_mid1:
            x1_pos = ui->mid1_text->x() + ui->mid1_text->width();
            y1_pos = ui->mid1_text->y() + ui->mid1_text->height() / 2;
            x2_pos = ui->right_text->x();
            y2_pos = ui->right_text->y() + ui->right_text->height() / 2;
            break;
        case right_mid2:
            x1_pos = ui->mid2_text->x() + ui->mid2_text->width();
            y1_pos = ui->mid2_text->y() + ui->mid2_text->height() / 2;
            x2_pos = ui->right_text->x();
            y2_pos = ui->right_text->y() + ui->right_text->height();
            break;
        case right_mid3:
            x1_pos = ui->right_text->x() + ui->left_text->width() / 2;
            y1_pos = ui->right_text->y() + ui->left_text->height();
            x2_pos = ui->mid3_text->x() + ui->mid3_text->width();
            y2_pos = ui->mid3_text->y() + ui->mid3_text->height() / 2;
            break;
        case mid1_mid2:
            x1_pos = ui->mid1_text->x() + ui->mid1_text->width() / 2;
            y1_pos = ui->mid1_text->y() + ui->mid1_text->height();
            x2_pos = ui->mid2_text->x() + ui->mid2_text->width() / 2;
            y2_pos = ui->mid2_text->y();
            break;
        case mid2_mid3:
            x1_pos = ui->mid2_text->x() + ui->mid2_text->width() / 2;
            y1_pos = ui->mid2_text->y() + ui->mid2_text->height();
            x2_pos = ui->mid3_text->x() + ui->mid3_text->width() / 2;
            y2_pos = ui->mid3_text->y();
            break;
        case mid1_mid3:
            x1_pos = ui->mid1_text->x() + ui->mid1_text->width() / 4;
            y1_pos = ui->mid1_text->y() + ui->mid1_text->height();
            x2_pos = ui->mid3_text->x() + ui->mid3_text->width() / 4;
            y2_pos = ui->mid3_text->y();
            break;

    }
}

/* Enable or disable distances selection */
void QDeepBreath::enableDistances(bool is_enabled) {
    ui->left_right_checkbox->setEnabled(is_enabled);
    ui->left_mid1_checkbox->setEnabled(is_enabled);
    ui->left_mid2_checkbox->setEnabled(is_enabled);
    ui->left_mid3_checkbox->setEnabled(is_enabled);
    ui->right_mid1_checkbox->setEnabled(is_enabled);
    ui->right_mid2_checkbox->setEnabled(is_enabled);
    ui->right_mid3_checkbox->setEnabled(is_enabled);
    ui->mid1_mid2_checkbox->setEnabled(is_enabled);
    ui->mid2_mid3_checkbox->setEnabled(is_enabled);
    ui->mid1_mid3_checkbox->setEnabled(is_enabled);

    ui->left_text->setEnabled(is_enabled);
    ui->right_text->setEnabled(is_enabled);
    ui->mid1_text->setEnabled(is_enabled);
    ui->mid2_text->setEnabled(is_enabled);
    ui->mid3_text->setEnabled(is_enabled);

    ui->distances_text->setEnabled(is_enabled);

	if (is_enabled) { //set according to config:
		setConfigDistances();
	}
	else { //clear checks:
		ui->left_right_checkbox->setChecked(is_enabled);
		ui->left_mid1_checkbox->setChecked(is_enabled);
		ui->left_mid2_checkbox->setChecked(is_enabled);
		ui->left_mid3_checkbox->setChecked(is_enabled);
		ui->right_mid1_checkbox->setChecked(is_enabled);
		ui->right_mid2_checkbox->setChecked(is_enabled);
		ui->right_mid3_checkbox->setChecked(is_enabled);
		ui->mid1_mid2_checkbox->setChecked(is_enabled);
		ui->mid2_mid3_checkbox->setChecked(is_enabled);
		ui->mid1_mid3_checkbox->setChecked(is_enabled);
	}

	this->update();

}

/* Enable or disable locations selection */
void QDeepBreath::enableLocations(bool is_enabled) {
    //set enabled:
    ui->left_loc_checkbox->setEnabled(is_enabled);
    ui->right_loc_checkbox->setEnabled(is_enabled);
    ui->mid1_loc_checkbox->setEnabled(is_enabled);
    ui->mid2_loc_checkbox->setEnabled(is_enabled);
    ui->mid3_loc_checkbox->setEnabled(is_enabled);

    ui->locations_text->setEnabled(is_enabled);

	if (is_enabled) { //set according to config:
		setConfigLocations();
	}
	else { //clear checks:
		ui->left_loc_checkbox->setChecked(is_enabled);
		ui->right_loc_checkbox->setChecked(is_enabled);
		ui->mid1_loc_checkbox->setChecked(is_enabled);
		ui->mid2_loc_checkbox->setChecked(is_enabled);
		ui->mid3_loc_checkbox->setChecked(is_enabled);
	}

	this->update();
}

/* Enable or disable menu */
void QDeepBreath::enableMenu(bool is_enabled) {
    ui->dimension_text->setEnabled(is_enabled);
    ui->dimension_2d_radio_button->setEnabled(is_enabled);
    ui->dimension_3d_radio_button->setEnabled(is_enabled);

    ui->mode_text->setEnabled(is_enabled);
    ui->mode_combo_box->setEnabled(is_enabled);

    ui->num_markers_text->setEnabled(is_enabled);
    ui->num_markers_combo_box->setEnabled(is_enabled);

    ui->is_stickers_checkbox->setEnabled(is_enabled);
    ui->b_color_radio_button->setEnabled(is_enabled);
    ui->g_color_radio_button->setEnabled(is_enabled);
    ui->y_color_radio_button->setEnabled(is_enabled);
}

void QDeepBreath::clearStreamingWidgets() {
	ui->depth_stream_widget->clear();
	ui->color_stream_widget->clear();
}

/* ==================== *
    CLICK HANDLERS:
 * ==================== */

/*  DISTANCES CHECKBOXES   */
void QDeepBreath::on_left_mid1_checkbox_clicked()
{
	DeepBreathConfig& user_cfg = DeepBreathConfig::getInstance();
	distances dist = distances::left_mid1;
	user_cfg.dists_included[dist] = ui->left_mid1_checkbox->isChecked();

	this->update();
}

void QDeepBreath::on_left_right_checkbox_clicked()
{
	DeepBreathConfig& user_cfg = DeepBreathConfig::getInstance();
	distances dist = distances::left_right;
	user_cfg.dists_included[dist] = ui->left_right_checkbox->isChecked();

	this->update();
}

void QDeepBreath::on_right_mid1_checkbox_clicked()
{
	DeepBreathConfig& user_cfg = DeepBreathConfig::getInstance();
	distances dist = distances::right_mid1;
	user_cfg.dists_included[dist] = ui->right_mid1_checkbox->isChecked();

    this->update();
}

void QDeepBreath::on_left_mid2_checkbox_clicked()
{
	DeepBreathConfig& user_cfg = DeepBreathConfig::getInstance();
	distances dist = distances::left_mid2;
	user_cfg.dists_included[dist] = ui->left_mid2_checkbox->isChecked();

    this->update();
}

void QDeepBreath::on_mid1_mid2_checkbox_clicked()
{
	DeepBreathConfig& user_cfg = DeepBreathConfig::getInstance();
	distances dist = distances::left_mid1;
	user_cfg.dists_included[dist] = ui->mid1_mid2_checkbox->isChecked();

    this->update();
}

void QDeepBreath::on_right_mid2_checkbox_clicked()
{
	DeepBreathConfig& user_cfg = DeepBreathConfig::getInstance();
	distances dist = distances::right_mid2;
	user_cfg.dists_included[dist] = ui->right_mid2_checkbox->isChecked();

    this->update();
}

void QDeepBreath::on_right_mid3_checkbox_clicked()
{
	DeepBreathConfig& user_cfg = DeepBreathConfig::getInstance();
	distances dist = distances::right_mid3;
	user_cfg.dists_included[dist] = ui->right_mid3_checkbox->isChecked();

    this->update();
}

void QDeepBreath::on_left_mid3_checkbox_clicked()
{
	DeepBreathConfig& user_cfg = DeepBreathConfig::getInstance();
	distances dist = distances::left_mid3;
	user_cfg.dists_included[dist] = ui->left_mid3_checkbox->isChecked();

    this->update();
}

void QDeepBreath::on_mid1_mid3_checkbox_clicked()
{
	DeepBreathConfig& user_cfg = DeepBreathConfig::getInstance();
	distances dist = distances::mid1_mid3;
	user_cfg.dists_included[dist] = ui->mid1_mid3_checkbox->isChecked();

    this->update();
}

void QDeepBreath::on_mid2_mid3_checkbox_clicked()
{
	DeepBreathConfig& user_cfg = DeepBreathConfig::getInstance();
	distances dist = distances::mid2_mid3;
	user_cfg.dists_included[dist] = ui->mid2_mid3_checkbox->isChecked();

    this->update();
}


/*  STREAM SOURCE BUTTONS   */

void QDeepBreath::on_start_camera_button_clicked()
{
	DeepBreathCamera& camera = DeepBreathCamera::getInstance();
	DeepBreathFrameManager& frame_manager = DeepBreathFrameManager::getInstance();

    if(!is_camera_on) {
        //if camera is off:
        // - disable file stream if open
        // - turn camera on
        // - change button's title
        // - show and enable recording
        if(is_run_from_file) {
            //TODO: Show alert "This will close the file stream. Continue?"
			QMessageBox::StandardButton reply;
			reply = QMessageBox::question(this, "Start Camera", "This action will stop the file stream. Continue?",
				QMessageBox::Ok | QMessageBox::Cancel);

			if (reply == QMessageBox::Ok) {
				//Close file stream (currently set in cfg)
				camera.cfg.disable_all_streams();
				camera.cfg = rs2::config();
				camera.pipe.stop();

				//reset filename, so that if "load file" is clicked again, a new explorer window will appear
				camera.filename = nullptr;

				//Close log file
				//DeepBreathLog& log = DeepBreathLog::getInstance();
				//assert(log); //log instance must be initiated before frame processing (i.e. "start camera" or "load file" before cv notify)
				//log.log_file.close();

				//turn streaming off and change title
				ui->load_file_button->setText("Load File...");

				//hide and disable pause button:
				ui->pause_button->setVisible(false);
				ui->pause_button->setEnabled(false);
				is_run_from_file = false;
			}
			if (reply == QMessageBox::Cancel)
			{
				return;
			}
        }
        ui->start_camera_button->setText("Stop Camera");
        ui->record_button->setVisible(true);
        ui->record_button->setEnabled(true);

        //diable change of menu:
        enableMenu(false);
		enableDistances(false);
		enableLocations(false);

		//start stream:
		camera.cfg.enable_stream(RS2_STREAM_DEPTH);
		camera.cfg.enable_stream(RS2_STREAM_COLOR, RS2_FORMAT_RGB8);
		camera.pipe.start(camera.cfg);

		frame_manager.reset(); // reset FrameManager for additional processing

		//TODO:
		//graph.reset(frame_manager.manager_start_time);

		//create logging:
		//std::string D2units = (DeepBreathConfig::getInstance().calc_2d_by_cm) ? "cm" : "pixels";
		//DeepBreathLog::createInstance(camera.filename, DeepBreathConfig::getInstance().num_of_stickers, D2units);
		DeepBreathLog::init(false);
		DeepBreathGraphPlot::createInstance(ui->graph_widget);

		//update condition variable to start polling:
		DeepBreathSync::is_poll_frame = true;
		DeepBreathSync::cv_poll_frame.notify_one();

        is_camera_on = true;
    }
    else {
        //if camera is on:
        // - turn camera off
        // - change button's title
        // - hide and disable recording

		//stop frame polling:
		DeepBreathSync::is_poll_frame = false;

        //Turn camera off:
		camera.cfg.disable_stream(RS2_STREAM_DEPTH);
		camera.cfg.disable_stream(RS2_STREAM_COLOR);
		camera.pipe.stop();

		//reset filename, so that if "load file" is clicked again, a new explorer window will appear
		camera.filename = nullptr;

		//Close log file
		//DeepBreathLog& log = DeepBreathLog::getInstance();
		//assert(log); //log instance must be initiated before frame processing (i.e. "start camera" or "load file" before cv notify)
		//log.log_file.close();

        ui->start_camera_button->setText("Start Camera");
        ui->record_button->setEnabled(false);
        ui->record_button->setVisible(false);

        //enable change of menu:
        enableMenu(true);
		enableDistances(true);
		enableLocations(true);
        is_camera_on = false;
    }
}

void QDeepBreath::on_record_button_clicked()
{
	DeepBreathCamera& camera = DeepBreathCamera::getInstance();

    if(!is_recording) {
        //turn recording on and change title
		camera.pipe.stop(); // Stop the pipeline with the default configuration
		const char* out_filename = rs2::file_dialog_open(rs2::file_dialog_mode::save_file, "ROS-bag\0*.bag\0", NULL, NULL);
		camera.cfg.enable_record_to_file(out_filename);
		camera.pipe.start(camera.cfg); //File will be opened at this point

        ui->record_button->setText("Stop Recording");
        is_recording = true;
    }
    else {
        //turn recording off and change title
		camera.cfg.disable_all_streams();
		camera.cfg = rs2::config();
		camera.pipe.stop(); // Stop the pipeline that holds the file and the recorder
		camera.pipe.start(camera.cfg);

        ui->record_button->setText("Record");
        is_recording = false;
    }
}

void QDeepBreath::on_load_file_button_clicked()
{
	DeepBreathCamera& camera = DeepBreathCamera::getInstance();
	DeepBreathFrameManager& frame_manager = DeepBreathFrameManager::getInstance();

    if(!is_run_from_file) {
        //if camera is on, show alert and stop stream
        if(is_camera_on) {
            //Show alert and stop camera stream if user clicked OK
			QMessageBox::StandardButton reply;
			reply = QMessageBox::question(this, "Load File", "This action will stop the camera stream. Continue?",
				QMessageBox::Ok | QMessageBox::Cancel);

			if (reply == QMessageBox::Ok) {
				if (is_recording) {
					//trigger stop recording:
					ui->record_button->click();
				}

				//trigger stop camera click (and hide record button):
				ui->start_camera_button->click();
			}
			if (reply == QMessageBox::Cancel)
			{
				return;
			}
        }

		//Load File
		camera.filename = rs2::file_dialog_open(rs2::file_dialog_mode::open_file, "ROS-bag\0*.bag\0", NULL, NULL);
		if (camera.filename) {
			camera.cfg.enable_device_from_file(camera.filename, FILE_ON_REPEAT);
			//start_time = clock();
			camera.pipe.start(camera.cfg); //File will be opened in read mode at this point

			frame_manager.reset(); // reset FrameManager for additional processing

			//create logging:
			//std::string D2units = (DeepBreathConfig::getInstance().calc_2d_by_cm) ? "cm" : "pixels";
			//DeepBreathLog::createInstance(camera.filename, DeepBreathConfig::getInstance().num_of_stickers, D2units);
			DeepBreathLog::init(true);
			DeepBreathGraphPlot::createInstance(ui->graph_widget);


			//show and enable pause button
			ui->pause_button->setVisible(true);
			ui->pause_button->setEnabled(true);

			//turn streaming off and change title
			ui->load_file_button->setText("Stop");

			//diable change of menu:
			enableMenu(false);
			enableDistances(false);
			enableLocations(false);

			//update condition variable to start polling:
			DeepBreathSync::is_poll_frame = true;
			DeepBreathSync::cv_poll_frame.notify_one();

			is_run_from_file = true;
		}
    }
    else {
		//stop frame polling and wait for notify from polling thread:
		DeepBreathSync::is_poll_frame = false;
		while (!DeepBreathSync::is_end_poll_frame) {
			std::unique_lock<std::mutex> lk(DeepBreathSync::m_end_poll_frame);
			DeepBreathSync::cv_end_poll_frame.wait_for(lk, 100ms);
			lk.unlock();
		}
		DeepBreathSync::is_end_poll_frame = false; // Reset boolean for the next time

		frame_manager.reset(); // reset FrameManager for additional processing
		DeepBreathGraphPlot::getInstance().reset();

		//turn streaming off and change title
		ui->load_file_button->setText("Load File...");

		camera.cfg.disable_all_streams();
		camera.cfg = rs2::config();
		camera.pipe.stop();

		//reset filename argument:
		camera.filename = nullptr;

		//Stop logging
		DeepBreathLog::stop();

		//clear stream widgets
		clearStreamingWidgets();

		//show and enable pause button
		ui->pause_button->setVisible(false);
		ui->pause_button->setEnabled(false);

		//enable change of menu:
		enableMenu(true);
		enableDistances(true);
		enableLocations(true);

		is_run_from_file = false;

    }
}

void QDeepBreath::on_pause_button_clicked()
{
	DeepBreathCamera& camera = DeepBreathCamera::getInstance();

    if(!is_pause) {
		DeepBreathSync::is_poll_frame = false;

        ui->pause_button->setText("Continue");

		rs2::device device = camera.pipe.get_active_profile().get_device();
		rs2::playback playback = device.as<rs2::playback>();
		playback.pause();

		//freez the last frame while pausing:
		if (camera.fs.size() > 0) {

			auto d = camera.fs.get_depth_frame();
			auto c = camera.fs.get_color_frame();

			std::map<int, rs2::frame> freeze_frames;
			freeze_frames[0] = camera.colorizer.process(c);
			freeze_frames[1] = camera.colorizer.process(d);

		}

        is_pause = true;
    }
    else { //currently paused, text is "Continue"
        ui->pause_button->setText("Pause");

		rs2::device device = camera.pipe.get_active_profile().get_device();
		rs2::playback playback = device.as<rs2::playback>();
		playback.resume();

		//update condition variable to start polling:
		DeepBreathSync::is_poll_frame = true;
		DeepBreathSync::cv_poll_frame.notify_one();

        is_pause = false;
    }
}

void QDeepBreath::on_mode_combo_box_currentIndexChanged(int index)
{

	DeepBreathConfig& user_cfg = DeepBreathConfig::getInstance();
	assert(user_cfg);

    switch(index) {
        case 0: //Distances
            enableDistances(true);
			setConfigDistances();
            enableLocations(false);
			ui->dimension_2d_radio_button->setEnabled(true);
			ui->dimension_3d_radio_button->setEnabled(true);
			ui->volume_type_combo_box->setVisible(false);
			setConfigDimension();
			user_cfg.mode = DISTANCES;
            break;
        case 1: //Locations
            enableDistances(false);
            enableLocations(true);
			setConfigLocations();
			ui->dimension_3d_radio_button->click();
			ui->dimension_2d_radio_button->setEnabled(false); //locations are only relevant in 3d
			ui->dimension_3d_radio_button->setEnabled(false);
			ui->volume_type_combo_box->setVisible(false);
			user_cfg.mode = LOCATION;
            break;
        case 2: //Fourier
            enableDistances(true);
			setConfigDistances();
            enableLocations(false);
			ui->dimension_2d_radio_button->setEnabled(true);
			ui->dimension_3d_radio_button->setEnabled(true);
			ui->volume_type_combo_box->setVisible(false);
			user_cfg.mode = FOURIER;
            break;
        case 3: //Volume
            enableDistances(false);
            enableLocations(false);
			ui->volume_type_combo_box->setVisible(true);
			ui->left_loc_checkbox->setEnabled(true);
			ui->right_loc_checkbox->setEnabled(true);
			ui->mid3_loc_checkbox->setEnabled(true);
			ui->left_loc_checkbox->click();
			ui->right_loc_checkbox->click();
			ui->mid3_loc_checkbox->click();
			ui->left_loc_checkbox->setEnabled(false);
			ui->right_loc_checkbox->setEnabled(false);
			ui->mid3_loc_checkbox->setEnabled(false);
			//only 3D allowed:
            ui->dimension_3d_radio_button->click();
            ui->dimension_3d_radio_button->setEnabled(false);
            ui->dimension_2d_radio_button->setEnabled(false);
			ui->num_markers_combo_box->setCurrentIndex(ui->num_markers_combo_box->findText("3"));
			user_cfg.stickers_included[stickers::left] = true;
			user_cfg.stickers_included[stickers::right] = true;
			user_cfg.stickers_included[stickers::mid3] = true;
			user_cfg.stickers_included[stickers::mid1] = false;
			user_cfg.stickers_included[stickers::mid2] = false;
			user_cfg.mode = VOLUME;
            break;
        case 4: //No Graph
            enableDistances(true);
			setConfigDistances();
            enableLocations(false);
			ui->dimension_2d_radio_button->setEnabled(true);
			ui->dimension_3d_radio_button->setEnabled(true);
			setConfigDimension();
			user_cfg.mode = NOGRAPH;
            break;
    }
}

void QDeepBreath::on_is_stickers_checkbox_clicked()
{
    if(ui->is_stickers_checkbox->isChecked()) {
        ui->b_color_radio_button->setEnabled(true);
        ui->g_color_radio_button->setEnabled(true);
        ui->y_color_radio_button->setEnabled(true);
    }
    else {
        ui->b_color_radio_button->setEnabled(false);
        ui->g_color_radio_button->setEnabled(false);
        ui->y_color_radio_button->setEnabled(false);
    }
}

void QDeepBreath::on_left_loc_checkbox_clicked()
{
	DeepBreathConfig& user_cfg = DeepBreathConfig::getInstance();
	assert(user_cfg);

	user_cfg.stickers_included[stickers::left] = ui->left_loc_checkbox->isChecked();
}

void QDeepBreath::on_mid1_loc_checkbox_clicked()
{
	DeepBreathConfig& user_cfg = DeepBreathConfig::getInstance();
	assert(user_cfg);

	user_cfg.stickers_included[stickers::mid1] = ui->mid1_loc_checkbox->isChecked();
}

void QDeepBreath::on_right_loc_checkbox_clicked()
{
	DeepBreathConfig& user_cfg = DeepBreathConfig::getInstance();
	assert(user_cfg);

	user_cfg.stickers_included[stickers::right] = ui->right_loc_checkbox->isChecked();
}

void QDeepBreath::on_mid2_loc_checkbox_clicked()
{
	DeepBreathConfig& user_cfg = DeepBreathConfig::getInstance();
	assert(user_cfg);

	user_cfg.stickers_included[stickers::mid2] = ui->mid2_loc_checkbox->isChecked();
}

void QDeepBreath::on_mid3_loc_checkbox_clicked()
{
	DeepBreathConfig& user_cfg = DeepBreathConfig::getInstance();
	assert(user_cfg);

	user_cfg.stickers_included[stickers::mid3] = ui->mid3_loc_checkbox->isChecked();
}

void QDeepBreath::on_dimension_2d_radio_button_clicked() {
	DeepBreathConfig& user_cfg = DeepBreathConfig::getInstance();
	assert(user_cfg);
	
	if (ui->dimension_2d_radio_button->isChecked()) {
		user_cfg.dimension = D2;
	}
}

void QDeepBreath::on_dimension_3d_radio_button_clicked() {
	DeepBreathConfig& user_cfg = DeepBreathConfig::getInstance();
	assert(user_cfg);

	if (ui->dimension_3d_radio_button->isChecked()) {
		user_cfg.dimension = D3;
	}
}

void QDeepBreath::on_num_markers_combo_box_currentIndexChanged(int index)
{
	DeepBreathConfig& user_cfg = DeepBreathConfig::getInstance();
	assert(user_cfg);

	switch (index) {
	case 0: //3
		user_cfg.num_of_stickers = 3;
		if (user_cfg.mode == LOCATION) {
			//uncheck irrelevant locations and clear selection from config:
			if (ui->mid1_loc_checkbox->isChecked()) {
				ui->mid1_loc_checkbox->click();
			}
			if (ui->mid2_loc_checkbox->isChecked()) {
				ui->mid2_loc_checkbox->click();
			}
			ui->mid1_loc_checkbox->setEnabled(false);
			ui->mid2_loc_checkbox->setEnabled(false);
		}
		break;
	case 1: //4
		user_cfg.num_of_stickers = 4;
		if (user_cfg.mode == LOCATION) {
			if (ui->mid1_loc_checkbox->isChecked()) {
				ui->mid1_loc_checkbox->click();
			}
			ui->mid1_loc_checkbox->setEnabled(false);
			ui->mid2_loc_checkbox->setEnabled(true);
		}
		break;
	case 2: //5
		user_cfg.num_of_stickers = 5;
		if (user_cfg.mode == LOCATION) {
			ui->mid1_loc_checkbox->setEnabled(true);
			ui->mid2_loc_checkbox->setEnabled(true);
		}
		break;
	}
}

void QDeepBreath::on_volume_type_combo_box_currentIndexChanged(int index)
{
	DeepBreathConfig& user_cfg = DeepBreathConfig::getInstance();
	assert(user_cfg);

	switch (index) {
	case 0: //Tetrahedron
		user_cfg.volume_type = TETRAHEDRON;
		break;
	case 1: //Reimann
		user_cfg.volume_type = REIMANN;
		break;
	}
}


