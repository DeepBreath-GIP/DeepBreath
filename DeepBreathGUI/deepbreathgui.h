#ifndef DEEPBREATH_H
#define DEEPBREATH_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class DeepBreath; }
QT_END_NAMESPACE

enum Distance {
    LEFT_RIGHT,
    LEFT_MID1,
    LEFT_MID2,
    LEFT_MID3,
    RIGHT_MID1,
    RIGHT_MID2,
    RIGHT_MID3,
    MID1_MID2,
    MID1_MID3,
    MID2_MID3
};

class DeepBreath : public QMainWindow
{
    Q_OBJECT

public:
    DeepBreath(QWidget *parent = nullptr);
    ~DeepBreath();
    void paintEvent(QPaintEvent* event);

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

private:
    Ui::DeepBreath *ui;

    bool is_camera_on;
    bool is_recording;
    bool is_run_from_file;
    bool is_pause;

	void initDefaultSelection();
    void setTransparentBackroundToText();
    void drawDistancesLines();
    void setXYPosOfDistance(int& x1_pos, int& y1_pos, int& x2_pos, int& y2_pos, Distance dist);
    void enableDistances(bool is_enabled);
    void enableLocations(bool is_enabled);
    void enableMenu(bool is_enabled);
};
#endif // DEEPBREATH_H
