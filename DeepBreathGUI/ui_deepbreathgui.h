/********************************************************************************
** Form generated from reading UI file 'deepbreathgui.ui'
**
** Created by: Qt User Interface Compiler version 5.15.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DEEPBREATHGUI_H
#define UI_DEEPBREATHGUI_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_DeepBreath
{
public:
    QWidget *centralwidget;
    QPushButton *start_camera_button;
    QPushButton *load_file_button;
    QPushButton *record_button;
    QPushButton *pause_button;
    QRadioButton *dimension_2d_radio_button;
    QRadioButton *dimension_3d_radio_button;
    QPlainTextEdit *dimension_text;
    QPlainTextEdit *mode_text;
    QComboBox *mode_combo_box;
    QPlainTextEdit *locations_text;
    QCheckBox *left_loc_checkbox;
    QCheckBox *right_loc_checkbox;
    QCheckBox *mid1_loc_checkbox;
    QCheckBox *mid2_loc_checkbox;
    QCheckBox *mid3_loc_checkbox;
    QPlainTextEdit *num_markers_text;
    QComboBox *num_markers_combo_box;
    QCheckBox *is_stickers_checkbox;
    QRadioButton *y_color_radio_button;
    QRadioButton *g_color_radio_button;
    QRadioButton *b_color_radio_button;
    QPlainTextEdit *distances_text;
    QPlainTextEdit *left_text;
    QPlainTextEdit *mid1_text;
    QPlainTextEdit *right_text;
    QPlainTextEdit *mid2_text;
    QPlainTextEdit *mid3_text;
    QCheckBox *left_mid1_checkbox;
    QCheckBox *right_mid1_checkbox;
    QCheckBox *left_right_checkbox;
    QCheckBox *mid1_mid2_checkbox;
    QCheckBox *mid2_mid3_checkbox;
    QCheckBox *right_mid2_checkbox;
    QCheckBox *left_mid2_checkbox;
    QCheckBox *mid1_mid3_checkbox;
    QCheckBox *left_mid3_checkbox;
    QCheckBox *right_mid3_checkbox;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *DeepBreath)
    {
        if (DeepBreath->objectName().isEmpty())
            DeepBreath->setObjectName(QString::fromUtf8("DeepBreath"));
        DeepBreath->resize(1024, 728);
        centralwidget = new QWidget(DeepBreath);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        start_camera_button = new QPushButton(centralwidget);
        start_camera_button->setObjectName(QString::fromUtf8("start_camera_button"));
        start_camera_button->setGeometry(QRect(10, 10, 93, 28));
        load_file_button = new QPushButton(centralwidget);
        load_file_button->setObjectName(QString::fromUtf8("load_file_button"));
        load_file_button->setGeometry(QRect(10, 40, 93, 28));
        record_button = new QPushButton(centralwidget);
        record_button->setObjectName(QString::fromUtf8("record_button"));
        record_button->setEnabled(false);
        record_button->setGeometry(QRect(10, 90, 93, 28));
        pause_button = new QPushButton(centralwidget);
        pause_button->setObjectName(QString::fromUtf8("pause_button"));
        pause_button->setEnabled(false);
        pause_button->setGeometry(QRect(10, 120, 93, 28));
        dimension_2d_radio_button = new QRadioButton(centralwidget);
        dimension_2d_radio_button->setObjectName(QString::fromUtf8("dimension_2d_radio_button"));
        dimension_2d_radio_button->setGeometry(QRect(230, 10, 51, 20));
        dimension_3d_radio_button = new QRadioButton(centralwidget);
        dimension_3d_radio_button->setObjectName(QString::fromUtf8("dimension_3d_radio_button"));
        dimension_3d_radio_button->setGeometry(QRect(290, 10, 51, 20));
        dimension_text = new QPlainTextEdit(centralwidget);
        dimension_text->setObjectName(QString::fromUtf8("dimension_text"));
        dimension_text->setGeometry(QRect(140, 10, 81, 31));
        dimension_text->setMouseTracking(false);
        dimension_text->setFrameShape(QFrame::NoFrame);
        dimension_text->setReadOnly(true);
        mode_text = new QPlainTextEdit(centralwidget);
        mode_text->setObjectName(QString::fromUtf8("mode_text"));
        mode_text->setGeometry(QRect(140, 40, 81, 31));
        mode_text->setMouseTracking(false);
        mode_text->setFrameShape(QFrame::NoFrame);
        mode_text->setReadOnly(true);
        mode_combo_box = new QComboBox(centralwidget);
        mode_combo_box->addItem(QString());
        mode_combo_box->addItem(QString());
        mode_combo_box->addItem(QString());
        mode_combo_box->addItem(QString());
        mode_combo_box->addItem(QString());
        mode_combo_box->setObjectName(QString::fromUtf8("mode_combo_box"));
        mode_combo_box->setGeometry(QRect(230, 40, 101, 22));
        locations_text = new QPlainTextEdit(centralwidget);
        locations_text->setObjectName(QString::fromUtf8("locations_text"));
        locations_text->setEnabled(false);
        locations_text->setGeometry(QRect(740, 10, 91, 31));
        locations_text->setMouseTracking(false);
        locations_text->setFrameShape(QFrame::NoFrame);
        locations_text->setReadOnly(true);
        left_loc_checkbox = new QCheckBox(centralwidget);
        left_loc_checkbox->setObjectName(QString::fromUtf8("left_loc_checkbox"));
        left_loc_checkbox->setEnabled(false);
        left_loc_checkbox->setGeometry(QRect(680, 50, 51, 20));
        right_loc_checkbox = new QCheckBox(centralwidget);
        right_loc_checkbox->setObjectName(QString::fromUtf8("right_loc_checkbox"));
        right_loc_checkbox->setEnabled(false);
        right_loc_checkbox->setGeometry(QRect(820, 50, 51, 20));
        mid1_loc_checkbox = new QCheckBox(centralwidget);
        mid1_loc_checkbox->setObjectName(QString::fromUtf8("mid1_loc_checkbox"));
        mid1_loc_checkbox->setEnabled(false);
        mid1_loc_checkbox->setGeometry(QRect(750, 50, 51, 20));
        mid2_loc_checkbox = new QCheckBox(centralwidget);
        mid2_loc_checkbox->setObjectName(QString::fromUtf8("mid2_loc_checkbox"));
        mid2_loc_checkbox->setEnabled(false);
        mid2_loc_checkbox->setGeometry(QRect(750, 90, 51, 20));
        mid3_loc_checkbox = new QCheckBox(centralwidget);
        mid3_loc_checkbox->setObjectName(QString::fromUtf8("mid3_loc_checkbox"));
        mid3_loc_checkbox->setEnabled(false);
        mid3_loc_checkbox->setGeometry(QRect(750, 130, 51, 20));
        num_markers_text = new QPlainTextEdit(centralwidget);
        num_markers_text->setObjectName(QString::fromUtf8("num_markers_text"));
        num_markers_text->setGeometry(QRect(140, 70, 81, 31));
        num_markers_text->setMouseTracking(false);
        num_markers_text->setFrameShape(QFrame::NoFrame);
        num_markers_text->setReadOnly(true);
        num_markers_combo_box = new QComboBox(centralwidget);
        num_markers_combo_box->addItem(QString());
        num_markers_combo_box->addItem(QString());
        num_markers_combo_box->addItem(QString());
        num_markers_combo_box->setObjectName(QString::fromUtf8("num_markers_combo_box"));
        num_markers_combo_box->setGeometry(QRect(230, 70, 41, 22));
        is_stickers_checkbox = new QCheckBox(centralwidget);
        is_stickers_checkbox->setObjectName(QString::fromUtf8("is_stickers_checkbox"));
        is_stickers_checkbox->setGeometry(QRect(140, 100, 101, 20));
        y_color_radio_button = new QRadioButton(centralwidget);
        y_color_radio_button->setObjectName(QString::fromUtf8("y_color_radio_button"));
        y_color_radio_button->setEnabled(false);
        y_color_radio_button->setGeometry(QRect(220, 100, 41, 20));
        g_color_radio_button = new QRadioButton(centralwidget);
        g_color_radio_button->setObjectName(QString::fromUtf8("g_color_radio_button"));
        g_color_radio_button->setEnabled(false);
        g_color_radio_button->setGeometry(QRect(260, 100, 41, 20));
        b_color_radio_button = new QRadioButton(centralwidget);
        b_color_radio_button->setObjectName(QString::fromUtf8("b_color_radio_button"));
        b_color_radio_button->setEnabled(false);
        b_color_radio_button->setGeometry(QRect(300, 100, 41, 20));
        distances_text = new QPlainTextEdit(centralwidget);
        distances_text->setObjectName(QString::fromUtf8("distances_text"));
        distances_text->setGeometry(QRect(450, 10, 91, 31));
        distances_text->setMouseTracking(false);
        distances_text->setFrameShape(QFrame::NoFrame);
        distances_text->setReadOnly(true);
        left_text = new QPlainTextEdit(centralwidget);
        left_text->setObjectName(QString::fromUtf8("left_text"));
        left_text->setGeometry(QRect(380, 50, 31, 31));
        left_text->setMouseTracking(false);
        left_text->setFrameShape(QFrame::NoFrame);
        left_text->setReadOnly(true);
        mid1_text = new QPlainTextEdit(centralwidget);
        mid1_text->setObjectName(QString::fromUtf8("mid1_text"));
        mid1_text->setGeometry(QRect(470, 50, 41, 31));
        mid1_text->setMouseTracking(false);
        mid1_text->setFrameShape(QFrame::NoFrame);
        mid1_text->setReadOnly(true);
        right_text = new QPlainTextEdit(centralwidget);
        right_text->setObjectName(QString::fromUtf8("right_text"));
        right_text->setGeometry(QRect(570, 50, 41, 31));
        right_text->setMouseTracking(false);
        right_text->setFrameShape(QFrame::NoFrame);
        right_text->setReadOnly(true);
        mid2_text = new QPlainTextEdit(centralwidget);
        mid2_text->setObjectName(QString::fromUtf8("mid2_text"));
        mid2_text->setGeometry(QRect(470, 100, 41, 31));
        mid2_text->setMouseTracking(false);
        mid2_text->setFrameShape(QFrame::NoFrame);
        mid2_text->setReadOnly(true);
        mid3_text = new QPlainTextEdit(centralwidget);
        mid3_text->setObjectName(QString::fromUtf8("mid3_text"));
        mid3_text->setGeometry(QRect(470, 150, 41, 31));
        mid3_text->setMouseTracking(false);
        mid3_text->setFrameShape(QFrame::NoFrame);
        mid3_text->setReadOnly(true);
        left_mid1_checkbox = new QCheckBox(centralwidget);
        left_mid1_checkbox->setObjectName(QString::fromUtf8("left_mid1_checkbox"));
        left_mid1_checkbox->setGeometry(QRect(430, 60, 16, 20));
        right_mid1_checkbox = new QCheckBox(centralwidget);
        right_mid1_checkbox->setObjectName(QString::fromUtf8("right_mid1_checkbox"));
        right_mid1_checkbox->setGeometry(QRect(530, 60, 16, 20));
        left_right_checkbox = new QCheckBox(centralwidget);
        left_right_checkbox->setObjectName(QString::fromUtf8("left_right_checkbox"));
        left_right_checkbox->setGeometry(QRect(480, 40, 16, 20));
        mid1_mid2_checkbox = new QCheckBox(centralwidget);
        mid1_mid2_checkbox->setObjectName(QString::fromUtf8("mid1_mid2_checkbox"));
        mid1_mid2_checkbox->setGeometry(QRect(500, 80, 16, 20));
        mid2_mid3_checkbox = new QCheckBox(centralwidget);
        mid2_mid3_checkbox->setObjectName(QString::fromUtf8("mid2_mid3_checkbox"));
        mid2_mid3_checkbox->setGeometry(QRect(500, 130, 16, 20));
        right_mid2_checkbox = new QCheckBox(centralwidget);
        right_mid2_checkbox->setObjectName(QString::fromUtf8("right_mid2_checkbox"));
        right_mid2_checkbox->setGeometry(QRect(530, 90, 16, 20));
        left_mid2_checkbox = new QCheckBox(centralwidget);
        left_mid2_checkbox->setObjectName(QString::fromUtf8("left_mid2_checkbox"));
        left_mid2_checkbox->setGeometry(QRect(430, 90, 16, 20));
        mid1_mid3_checkbox = new QCheckBox(centralwidget);
        mid1_mid3_checkbox->setObjectName(QString::fromUtf8("mid1_mid3_checkbox"));
        mid1_mid3_checkbox->setGeometry(QRect(460, 130, 16, 20));
        left_mid3_checkbox = new QCheckBox(centralwidget);
        left_mid3_checkbox->setObjectName(QString::fromUtf8("left_mid3_checkbox"));
        left_mid3_checkbox->setGeometry(QRect(420, 110, 16, 20));
        right_mid3_checkbox = new QCheckBox(centralwidget);
        right_mid3_checkbox->setObjectName(QString::fromUtf8("right_mid3_checkbox"));
        right_mid3_checkbox->setGeometry(QRect(550, 110, 16, 20));
        DeepBreath->setCentralWidget(centralwidget);
        menubar = new QMenuBar(DeepBreath);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 1024, 26));
        DeepBreath->setMenuBar(menubar);
        statusbar = new QStatusBar(DeepBreath);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        DeepBreath->setStatusBar(statusbar);

        retranslateUi(DeepBreath);

        QMetaObject::connectSlotsByName(DeepBreath);
    } // setupUi

    void retranslateUi(QMainWindow *DeepBreath)
    {
        DeepBreath->setWindowTitle(QCoreApplication::translate("DeepBreath", "DeepBreath", nullptr));
        start_camera_button->setText(QCoreApplication::translate("DeepBreath", "Start Camera", nullptr));
        load_file_button->setText(QCoreApplication::translate("DeepBreath", "Load File...", nullptr));
        record_button->setText(QCoreApplication::translate("DeepBreath", "Record", nullptr));
        pause_button->setText(QCoreApplication::translate("DeepBreath", "Pause", nullptr));
        dimension_2d_radio_button->setText(QCoreApplication::translate("DeepBreath", "2D", nullptr));
        dimension_3d_radio_button->setText(QCoreApplication::translate("DeepBreath", "3D", nullptr));
        dimension_text->setPlainText(QCoreApplication::translate("DeepBreath", "Dimesion:", nullptr));
        mode_text->setPlainText(QCoreApplication::translate("DeepBreath", "Mode:", nullptr));
        mode_combo_box->setItemText(0, QCoreApplication::translate("DeepBreath", "Distances", nullptr));
        mode_combo_box->setItemText(1, QCoreApplication::translate("DeepBreath", "Locations", nullptr));
        mode_combo_box->setItemText(2, QCoreApplication::translate("DeepBreath", "Fourier", nullptr));
        mode_combo_box->setItemText(3, QCoreApplication::translate("DeepBreath", "Volume", nullptr));
        mode_combo_box->setItemText(4, QCoreApplication::translate("DeepBreath", "No Graph", nullptr));

        locations_text->setPlainText(QCoreApplication::translate("DeepBreath", "Locations:", nullptr));
        left_loc_checkbox->setText(QCoreApplication::translate("DeepBreath", "Left", nullptr));
        right_loc_checkbox->setText(QCoreApplication::translate("DeepBreath", "Right", nullptr));
        mid1_loc_checkbox->setText(QCoreApplication::translate("DeepBreath", "mid1", nullptr));
        mid2_loc_checkbox->setText(QCoreApplication::translate("DeepBreath", "mid2", nullptr));
        mid3_loc_checkbox->setText(QCoreApplication::translate("DeepBreath", "mid3", nullptr));
        num_markers_text->setPlainText(QCoreApplication::translate("DeepBreath", "#Markers:", nullptr));
        num_markers_combo_box->setItemText(0, QCoreApplication::translate("DeepBreath", "3", nullptr));
        num_markers_combo_box->setItemText(1, QCoreApplication::translate("DeepBreath", "4", nullptr));
        num_markers_combo_box->setItemText(2, QCoreApplication::translate("DeepBreath", "5", nullptr));

        is_stickers_checkbox->setText(QCoreApplication::translate("DeepBreath", "Stickers", nullptr));
        y_color_radio_button->setText(QCoreApplication::translate("DeepBreath", "Y", nullptr));
        g_color_radio_button->setText(QCoreApplication::translate("DeepBreath", "G", nullptr));
        b_color_radio_button->setText(QCoreApplication::translate("DeepBreath", "B", nullptr));
        distances_text->setPlainText(QCoreApplication::translate("DeepBreath", "Distances:", nullptr));
        left_text->setPlainText(QCoreApplication::translate("DeepBreath", "Left", nullptr));
        mid1_text->setPlainText(QCoreApplication::translate("DeepBreath", "mid1", nullptr));
        right_text->setPlainText(QCoreApplication::translate("DeepBreath", "Right", nullptr));
        mid2_text->setPlainText(QCoreApplication::translate("DeepBreath", "mid2", nullptr));
        mid3_text->setPlainText(QCoreApplication::translate("DeepBreath", "mid3", nullptr));
        left_mid1_checkbox->setText(QString());
        right_mid1_checkbox->setText(QString());
        left_right_checkbox->setText(QString());
        mid1_mid2_checkbox->setText(QString());
        mid2_mid3_checkbox->setText(QString());
        right_mid2_checkbox->setText(QString());
        left_mid2_checkbox->setText(QString());
        mid1_mid3_checkbox->setText(QString());
        left_mid3_checkbox->setText(QString());
        right_mid3_checkbox->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class DeepBreath: public Ui_DeepBreath {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DEEPBREATHGUI_H