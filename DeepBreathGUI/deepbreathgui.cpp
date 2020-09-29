#include "deepbreathgui.h"
#include "ui_deepbreathgui.h"
#include <Qt>
#include <QPainter>
#include <QPen>
#include <QLine>

/* DeepBreath Files */
#include "db_config.hpp"

//NOTE FOR SELF:
//To Debug on release mode:
//https://docs.microsoft.com/en-us/cpp/build/how-to-debug-a-release-build?view=vs-2019

DeepBreath::DeepBreath(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::DeepBreath)
    , is_camera_on(false)
    , is_recording(false)
    , is_run_from_file(false)
    , is_pause(false)
{
    ui->setupUi(this);

    setTransparentBackroundToText();
    ui->record_button->setVisible(false);
    ui->pause_button->setVisible(false);

	initDefaultSelection();
}

DeepBreath::~DeepBreath()
{
    delete ui;
}

void DeepBreath::paintEvent(QPaintEvent* event) {
    drawDistancesLines();
}

/*	Initiates the UI to default selection as configured in the config file. */
void DeepBreath::initDefaultSelection() {
	//create and get instance of the config.
	//TODO: remove config_err
	std::string config_err;
	DeepBreathConfig::createInstance(CONFIG_FILEPATH, &config_err);
	DeepBreathConfig user_cfg = DeepBreathConfig::getInstance();
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

	//set mode:
	int mode_index = -1; //initialize to invalid index by default
	switch (user_cfg.mode) {
	case DISTANCES:
		mode_index = ui->mode_combo_box->findText("Distances");
		break;
	case LOCATION:
		mode_index = ui->mode_combo_box->findText("Locations");
		break;
	case FOURIER:
		mode_index = ui->mode_combo_box->findText("Fourier");
		break;
	case VOLUME:
		mode_index = ui->mode_combo_box->findText("Volume");
		break;
	case NOGRAPH:
		mode_index = ui->mode_combo_box->findText("No Graph");
		break;
	}
	ui->mode_combo_box->setCurrentIndex(mode_index);

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
	ui->is_stickers_checkbox->setChecked(user_cfg.is_stickers);
}

/* Removes backgrounds from texts: */
void DeepBreath::setTransparentBackroundToText() {
    ui->dimension_text->viewport()->setAutoFillBackground(false);
    ui->distances_text->viewport()->setAutoFillBackground(false);
    ui->locations_text->viewport()->setAutoFillBackground(false);
    ui->mode_text->viewport()->setAutoFillBackground(false);
    ui->num_markers_text->viewport()->setAutoFillBackground(false);
    ui->left_text->viewport()->setAutoFillBackground(false);
    ui->right_text->viewport()->setAutoFillBackground(false);
    ui->mid1_text->viewport()->setAutoFillBackground(false);
    ui->mid2_text->viewport()->setAutoFillBackground(false);
    ui->mid3_text->viewport()->setAutoFillBackground(false);

    enableLocations(false);
}

/* Draw Lines of distances to choose */
void DeepBreath::drawDistancesLines() {
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

    setXYPosOfDistance(x1_pos, y1_pos, x2_pos, y2_pos, LEFT_MID1);
    QLine left_mid1(x1_pos, y1_pos, x2_pos, y2_pos);

    setXYPosOfDistance(x1_pos, y1_pos, x2_pos, y2_pos, RIGHT_MID1);
    QLine right_mid1(x1_pos, y1_pos, x2_pos, y2_pos);

    setXYPosOfDistance(x1_pos, y1_pos, x2_pos, y2_pos, LEFT_MID2);
    QLine left_mid2(x1_pos, y1_pos, x2_pos, y2_pos);

    setXYPosOfDistance(x1_pos, y1_pos, x2_pos, y2_pos, RIGHT_MID2);
    QLine right_mid2(x1_pos, y1_pos, x2_pos, y2_pos);

    setXYPosOfDistance(x1_pos, y1_pos, x2_pos, y2_pos, LEFT_MID3);
    QLine left_mid3(x1_pos, y1_pos, x2_pos, y2_pos);

    setXYPosOfDistance(x1_pos, y1_pos, x2_pos, y2_pos, RIGHT_MID3);
    QLine right_mid3(x1_pos, y1_pos, x2_pos, y2_pos);

    setXYPosOfDistance(x1_pos, y1_pos, x2_pos, y2_pos, LEFT_RIGHT);
    QLine left_right(x1_pos, y1_pos, x2_pos, y2_pos);

    setXYPosOfDistance(x1_pos, y1_pos, x2_pos, y2_pos, MID1_MID2);
    QLine mid1_mid2(x1_pos, y1_pos, x2_pos, y2_pos);

    setXYPosOfDistance(x1_pos, y1_pos, x2_pos, y2_pos, MID2_MID3);
    QLine mid2_mid3(x1_pos, y1_pos, x2_pos, y2_pos);

    setXYPosOfDistance(x1_pos, y1_pos, x2_pos, y2_pos, MID1_MID3);
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

void DeepBreath::setXYPosOfDistance(int& x1_pos, int& y1_pos, int& x2_pos, int& y2_pos,
                                           Distance dist) {
    switch (dist) {
        case LEFT_RIGHT:
            x1_pos = ui->left_text->x() + ui->left_text->width() / 2;
            y1_pos = ui->left_text->y();
            x2_pos = ui->right_text->x() + ui->right_text->width() / 2;
            y2_pos = ui->right_text->y();
            break;
        case LEFT_MID1:
            x1_pos = ui->left_text->x() + ui->left_text->width();
            y1_pos = ui->left_text->y() + ui->left_text->height() / 2;
            x2_pos = ui->mid1_text->x();
            y2_pos = ui->mid1_text->y() + ui->mid1_text->height() / 2;
            break;
        case LEFT_MID2:
            x1_pos = ui->left_text->x() + ui->left_text->width();
            y1_pos = ui->left_text->y() + ui->left_text->height();
            x2_pos = ui->mid2_text->x();
            y2_pos = ui->mid2_text->y() + ui->mid1_text->height() / 2;
            break;
        case LEFT_MID3:
            x1_pos = ui->left_text->x() + ui->left_text->width() / 2;
            y1_pos = ui->left_text->y() + ui->left_text->height();
            x2_pos = ui->mid3_text->x();
            y2_pos = ui->mid3_text->y() + ui->mid3_text->height() / 2;
            break;
        case RIGHT_MID1:
            x1_pos = ui->mid1_text->x() + ui->mid1_text->width();
            y1_pos = ui->mid1_text->y() + ui->mid1_text->height() / 2;
            x2_pos = ui->right_text->x();
            y2_pos = ui->right_text->y() + ui->right_text->height() / 2;
            break;
        case RIGHT_MID2:
            x1_pos = ui->mid2_text->x() + ui->mid2_text->width();
            y1_pos = ui->mid2_text->y() + ui->mid2_text->height() / 2;
            x2_pos = ui->right_text->x();
            y2_pos = ui->right_text->y() + ui->right_text->height();
            break;
        case RIGHT_MID3:
            x1_pos = ui->right_text->x() + ui->left_text->width() / 2;
            y1_pos = ui->right_text->y() + ui->left_text->height();
            x2_pos = ui->mid3_text->x() + ui->mid3_text->width();
            y2_pos = ui->mid3_text->y() + ui->mid3_text->height() / 2;
            break;
        case MID1_MID2:
            x1_pos = ui->mid1_text->x() + ui->mid1_text->width() / 2;
            y1_pos = ui->mid1_text->y() + ui->mid1_text->height();
            x2_pos = ui->mid2_text->x() + ui->mid2_text->width() / 2;
            y2_pos = ui->mid2_text->y();
            break;
        case MID2_MID3:
            x1_pos = ui->mid2_text->x() + ui->mid2_text->width() / 2;
            y1_pos = ui->mid2_text->y() + ui->mid2_text->height();
            x2_pos = ui->mid3_text->x() + ui->mid3_text->width() / 2;
            y2_pos = ui->mid3_text->y();
            break;
        case MID1_MID3:
            x1_pos = ui->mid1_text->x() + ui->mid1_text->width() / 4;
            y1_pos = ui->mid1_text->y() + ui->mid1_text->height();
            x2_pos = ui->mid3_text->x() + ui->mid3_text->width() / 4;
            y2_pos = ui->mid3_text->y();
            break;

    }
}

/* Enable or disable distances selection */
void DeepBreath::enableDistances(bool is_enabled) {
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
}

/* Enable or disable locations selection */
void DeepBreath::enableLocations(bool is_enabled) {
    //set enabled:
    ui->left_loc_checkbox->setEnabled(is_enabled);
    ui->right_loc_checkbox->setEnabled(is_enabled);
    ui->mid1_loc_checkbox->setEnabled(is_enabled);
    ui->mid2_loc_checkbox->setEnabled(is_enabled);
    ui->mid3_loc_checkbox->setEnabled(is_enabled);

    ui->locations_text->setEnabled(is_enabled);
}

/* Enable or disable menu */
void DeepBreath::enableMenu(bool is_enabled) {
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

/* ==================== *
    CLICK HANDLERS:
 * ==================== */

/*  DISTANCES CHECKBOXES   */
void DeepBreath::on_left_mid1_checkbox_clicked()
{
   this->update();
}

void DeepBreath::on_left_right_checkbox_clicked()
{
    this->update();
}

void DeepBreath::on_right_mid1_checkbox_clicked()
{
    this->update();
}

void DeepBreath::on_left_mid2_checkbox_clicked()
{
    this->update();
}

void DeepBreath::on_mid1_mid2_checkbox_clicked()
{
    this->update();
}

void DeepBreath::on_right_mid2_checkbox_clicked()
{
    this->update();
}

void DeepBreath::on_right_mid3_checkbox_clicked()
{
    this->update();
}

void DeepBreath::on_left_mid3_checkbox_clicked()
{
    this->update();
}

void DeepBreath::on_mid1_mid3_checkbox_clicked()
{
    this->update();
}

void DeepBreath::on_mid2_mid3_checkbox_clicked()
{
    this->update();
}


/*  STREAM SOURCE BUTTONS   */

void DeepBreath::on_start_camera_button_clicked()
{
    if(!is_camera_on) {
        //if camera is off:
        // - disable file stream if open
        // - turn camera on
        // - change button's title
        // - show and enable recording
        //TODO: Turn camera on
        if(is_run_from_file) {
            //TODO: Show alert "This will close the file stream. Continue?"
            //TODO: Close file stream

            //turn streaming off and change title
            ui->load_file_button->setText("Load File...");

            //hide and disable pause button:
            ui->pause_button->setVisible(false);
            ui->pause_button->setEnabled(false);
            is_run_from_file = false;
        }
        ui->start_camera_button->setText("Stop Camera");
        ui->record_button->setVisible(true);
        ui->record_button->setEnabled(true);

        //diable change of menu:
        enableMenu(false);
        is_camera_on = true;
    }
    else {
        //if camera is on:
        // - turn camera off
        // - change button's title
        // - hide and disable recording
        //TODO: Turn camera off
        ui->start_camera_button->setText("Start Camera");
        ui->record_button->setEnabled(false);
        ui->record_button->setVisible(false);

        //enable change of menu:
        enableMenu(true);
        is_camera_on = false;
    }
}

void DeepBreath::on_record_button_clicked()
{
    if(!is_recording) {
        //turn recording on and change title
        ui->record_button->setText("Stop Recording");
        is_recording = true;
    }
    else {
        //turn recording off and change title
        ui->record_button->setText("Record");
        is_recording = false;
    }
}

void DeepBreath::on_load_file_button_clicked()
{
    if(!is_run_from_file) {
        //if camera is on, show alert and stop stream
        if(is_camera_on) {
            //TODO: Show alert and stop camera stream
            //trigger stop camera click (and hide record button):
            ui->start_camera_button->click();
            if(is_recording) {
                //trigger stop recording:
                ui->record_button->click();
            }
        }
        //TODO: Load File
        //show and enable pause button
        ui->pause_button->setVisible(true);
        ui->pause_button->setEnabled(true);

        //turn streaming off and change title
        ui->load_file_button->setText("Stop");

        //diable change of menu:
        enableMenu(false);
        is_run_from_file = true;
    }
    else {
        //turn streaming off and change title
        ui->load_file_button->setText("Load File...");
        //enable change of menu:
        enableMenu(true);
        is_run_from_file = false;
    }
}

void DeepBreath::on_pause_button_clicked()
{
    if(!is_pause) {
        ui->pause_button->setText("Continue");
        is_pause = true;
    }
    else {
        ui->pause_button->setText("Pause");
        is_pause = false;
    }
}

void DeepBreath::on_mode_combo_box_currentIndexChanged(int index)
{
    switch(index) {
        case 0: //Distances
            enableDistances(true);
            enableLocations(false);
            break;
        case 1: //Locations
            enableDistances(false);
            enableLocations(true);
            break;
        case 2: //Fourier
            enableDistances(true);
            enableLocations(false);
            break;
        case 3: //Volume
            enableDistances(false);
            enableLocations(false);
            ui->dimension_3d_radio_button->setChecked(true);
            ui->dimension_2d_radio_button->setEnabled(false);
            ui->is_stickers_checkbox->setEnabled(false);
            break;
        case 4: //No Graph
            enableDistances(true);
            enableLocations(false);
            break;
    }
}
