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

private:
    Ui::DeepBreath *ui;

    void setTransparentBackroundToText();
    void drawDistancesLines();
    void setXYPosOfDistance(int& x1_pos, int& y1_pos, int& x2_pos, int& y2_pos, Distance dist);
};
#endif // DEEPBREATH_H
