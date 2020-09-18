#include "deepbreath.h"
#include "ui_deepbreath.h"
#include <Qt>
#include <QPainter>
#include <QPen>
#include <QLine>

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
}

DeepBreath::~DeepBreath()
{
    delete ui;
}

void DeepBreath::paintEvent(QPaintEvent* event) {
    drawDistancesLines();
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
            //hide and disable pause button:
            ui->pause_button->setVisible(false);
            ui->pause_button->setEnabled(false);
            is_run_from_file = false;
        }
        ui->start_camera_button->setText("Stop Camera");
        ui->record_button->setVisible(true);
        ui->record_button->setEnabled(true);
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
        is_run_from_file = true;
    }
    else {
        //TODO: Close current file and open new file.
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
