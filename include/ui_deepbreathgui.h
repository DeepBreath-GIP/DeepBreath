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
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>
#include "custom_opengl_widget.hpp"
#include "qcustomplot.hpp"

#include "synchronized_q3dscatter.hpp"
#include "scatterdatamodifier.hpp"

using namespace QtDataVisualization;


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
	QLabel *dimension_text;
	QLabel *mode_text;
    QComboBox *mode_combo_box;
    QComboBox *volume_type_combo_box;
	QLabel *locations_text;
    QCheckBox *left_loc_checkbox;
    QCheckBox *right_loc_checkbox;
    QCheckBox *mid1_loc_checkbox;
    QCheckBox *mid2_loc_checkbox;
    QCheckBox *mid3_loc_checkbox;
	QLabel *num_markers_text;
    QComboBox *num_markers_combo_box;
    QCheckBox *is_stickers_checkbox;
    QRadioButton *y_color_radio_button;
    QRadioButton *g_color_radio_button;
    QRadioButton *b_color_radio_button;
	QLabel *distances_text;
	QLabel *left_text;
	QLabel *mid1_text;
	QLabel *right_text;
	QLabel *mid2_text;
	QLabel *mid3_text;
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
    CustomOpenGLWidget *color_stream_widget;
    CustomOpenGLWidget *depth_stream_widget;
    SynchronizedQ3DScatter* volume_scatter;
    ScatterDataModifier* volume_scatter_modifier;
    QWidget *volume_stream_widget;
    QWidget* volume_widget_container;
    QHBoxLayout* volume_hLayout;
    QVBoxLayout* volume_vLayout;
	QCustomPlot *graph_widget;
	QLabel *bpm_text;
	QLabel *bpm_value;
    QLabel* fps_text;
    QLabel* fps_value;
    QMenuBar *menubar;
    QStatusBar *statusbar;
    QButtonGroup *dimension_button_group;
    QButtonGroup *color_button_group;

    void setupUi(QMainWindow *DeepBreath)
    {
        if (DeepBreath->objectName().isEmpty())
            DeepBreath->setObjectName(QString::fromUtf8("DeepBreath"));
        DeepBreath->resize(1280, 768);
        centralwidget = new QWidget(DeepBreath);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        start_camera_button = new QPushButton(centralwidget);
        start_camera_button->setObjectName(QString::fromUtf8("start_camera_button"));
        start_camera_button->setGeometry(QRect(20, 20, 93, 28));
        load_file_button = new QPushButton(centralwidget);
        load_file_button->setObjectName(QString::fromUtf8("load_file_button"));
        load_file_button->setGeometry(QRect(20, 50, 93, 28));
        record_button = new QPushButton(centralwidget);
        record_button->setObjectName(QString::fromUtf8("record_button"));
        record_button->setEnabled(false);
        record_button->setGeometry(QRect(20, 100, 93, 28));
        pause_button = new QPushButton(centralwidget);
        pause_button->setObjectName(QString::fromUtf8("pause_button"));
        pause_button->setEnabled(false);
        pause_button->setGeometry(QRect(20, 130, 93, 28));
        dimension_2d_radio_button = new QRadioButton(centralwidget);
        dimension_button_group = new QButtonGroup(DeepBreath);
        dimension_button_group->setObjectName(QString::fromUtf8("dimension_button_group"));
        dimension_button_group->addButton(dimension_2d_radio_button);
        dimension_2d_radio_button->setObjectName(QString::fromUtf8("dimension_2d_radio_button"));
        dimension_2d_radio_button->setGeometry(QRect(210, 20, 51, 20));
        dimension_3d_radio_button = new QRadioButton(centralwidget);
        dimension_button_group->addButton(dimension_3d_radio_button);
        dimension_3d_radio_button->setObjectName(QString::fromUtf8("dimension_3d_radio_button"));
        dimension_3d_radio_button->setGeometry(QRect(270, 20, 51, 20));
        dimension_text = new QLabel(centralwidget);
        dimension_text->setObjectName(QString::fromUtf8("dimension_text"));
        dimension_text->setGeometry(QRect(130, 20, 81, 31));
        dimension_text->setMouseTracking(false);
        dimension_text->setFrameShape(QFrame::NoFrame);
        mode_text = new QLabel(centralwidget);
        mode_text->setObjectName(QString::fromUtf8("mode_text"));
        mode_text->setGeometry(QRect(130, 60, 81, 31));
        mode_text->setMouseTracking(false);
        mode_text->setFrameShape(QFrame::NoFrame);
        mode_combo_box = new QComboBox(centralwidget);
        mode_combo_box->addItem(QString());
        mode_combo_box->addItem(QString());
        mode_combo_box->addItem(QString());
        mode_combo_box->addItem(QString());
        mode_combo_box->addItem(QString());
        mode_combo_box->setObjectName(QString::fromUtf8("mode_combo_box"));
        mode_combo_box->setGeometry(QRect(210, 60, 91, 22));
        volume_type_combo_box = new QComboBox(centralwidget);
        volume_type_combo_box->addItem(QString());
        volume_type_combo_box->addItem(QString());
        volume_type_combo_box->setObjectName(QString::fromUtf8("volume_type_combo_box"));
        volume_type_combo_box->setGeometry(QRect(310, 60, 91, 22));
        locations_text = new QLabel(centralwidget);
        locations_text->setObjectName(QString::fromUtf8("locations_text"));
        locations_text->setEnabled(false);
        locations_text->setGeometry(QRect(120, 470, 91, 31));
        locations_text->setMouseTracking(false);
        locations_text->setFrameShape(QFrame::NoFrame);
        left_loc_checkbox = new QCheckBox(centralwidget);
        left_loc_checkbox->setObjectName(QString::fromUtf8("left_loc_checkbox"));
        left_loc_checkbox->setEnabled(false);
        left_loc_checkbox->setGeometry(QRect(60, 510, 51, 20));
        right_loc_checkbox = new QCheckBox(centralwidget);
        right_loc_checkbox->setObjectName(QString::fromUtf8("right_loc_checkbox"));
        right_loc_checkbox->setEnabled(false);
        right_loc_checkbox->setGeometry(QRect(200, 510, 51, 20));
        mid1_loc_checkbox = new QCheckBox(centralwidget);
        mid1_loc_checkbox->setObjectName(QString::fromUtf8("mid1_loc_checkbox"));
        mid1_loc_checkbox->setEnabled(false);
        mid1_loc_checkbox->setGeometry(QRect(130, 510, 51, 20));
        mid2_loc_checkbox = new QCheckBox(centralwidget);
        mid2_loc_checkbox->setObjectName(QString::fromUtf8("mid2_loc_checkbox"));
        mid2_loc_checkbox->setEnabled(false);
        mid2_loc_checkbox->setGeometry(QRect(130, 550, 51, 20));
        mid3_loc_checkbox = new QCheckBox(centralwidget);
        mid3_loc_checkbox->setObjectName(QString::fromUtf8("mid3_loc_checkbox"));
        mid3_loc_checkbox->setEnabled(false);
        mid3_loc_checkbox->setGeometry(QRect(130, 590, 51, 20));
        num_markers_text = new QLabel(centralwidget);
        num_markers_text->setObjectName(QString::fromUtf8("num_markers_text"));
        num_markers_text->setGeometry(QRect(130, 100, 81, 31));
        num_markers_text->setMouseTracking(false);
        num_markers_text->setFrameShape(QFrame::NoFrame);
        num_markers_combo_box = new QComboBox(centralwidget);
        num_markers_combo_box->addItem(QString());
        num_markers_combo_box->addItem(QString());
        num_markers_combo_box->addItem(QString());
        num_markers_combo_box->setObjectName(QString::fromUtf8("num_markers_combo_box"));
        num_markers_combo_box->setGeometry(QRect(210, 100, 41, 22));
        is_stickers_checkbox = new QCheckBox(centralwidget);
        is_stickers_checkbox->setObjectName(QString::fromUtf8("is_stickers_checkbox"));
        is_stickers_checkbox->setGeometry(QRect(130, 140, 101, 20));
        y_color_radio_button = new QRadioButton(centralwidget);
        color_button_group = new QButtonGroup(DeepBreath);
        color_button_group->setObjectName(QString::fromUtf8("color_button_group"));
        color_button_group->addButton(y_color_radio_button);
        y_color_radio_button->setObjectName(QString::fromUtf8("y_color_radio_button"));
        y_color_radio_button->setEnabled(false);
        y_color_radio_button->setGeometry(QRect(210, 140, 41, 20));
        g_color_radio_button = new QRadioButton(centralwidget);
        color_button_group->addButton(g_color_radio_button);
        g_color_radio_button->setObjectName(QString::fromUtf8("g_color_radio_button"));
        g_color_radio_button->setEnabled(false);
        g_color_radio_button->setGeometry(QRect(250, 140, 41, 20));
        b_color_radio_button = new QRadioButton(centralwidget);
        color_button_group->addButton(b_color_radio_button);
        b_color_radio_button->setObjectName(QString::fromUtf8("b_color_radio_button"));
        b_color_radio_button->setEnabled(false);
        b_color_radio_button->setGeometry(QRect(290, 140, 41, 20));
        distances_text = new QLabel(centralwidget);
        distances_text->setObjectName(QString::fromUtf8("distances_text"));
        distances_text->setGeometry(QRect(120, 230, 91, 31));
        distances_text->setMouseTracking(false);
        distances_text->setFrameShape(QFrame::NoFrame);
        left_text = new QLabel(centralwidget);
        left_text->setObjectName(QString::fromUtf8("left_text"));
        left_text->setGeometry(QRect(50, 270, 31, 31));
        left_text->setMouseTracking(false);
        left_text->setFrameShape(QFrame::NoFrame);
        mid1_text = new QLabel(centralwidget);
        mid1_text->setObjectName(QString::fromUtf8("mid1_text"));
        mid1_text->setGeometry(QRect(140, 270, 41, 31));
        mid1_text->setMouseTracking(false);
        mid1_text->setFrameShape(QFrame::NoFrame);
        right_text = new QLabel(centralwidget);
        right_text->setObjectName(QString::fromUtf8("right_text"));
        right_text->setGeometry(QRect(240, 270, 41, 31));
        right_text->setMouseTracking(false);
        right_text->setFrameShape(QFrame::NoFrame);
        mid2_text = new QLabel(centralwidget);
        mid2_text->setObjectName(QString::fromUtf8("mid2_text"));
        mid2_text->setGeometry(QRect(140, 320, 41, 31));
        mid2_text->setMouseTracking(false);
        mid2_text->setFrameShape(QFrame::NoFrame);
        mid3_text = new QLabel(centralwidget);
        mid3_text->setObjectName(QString::fromUtf8("mid3_text"));
        mid3_text->setGeometry(QRect(140, 370, 41, 31));
        mid3_text->setMouseTracking(false);
        mid3_text->setFrameShape(QFrame::NoFrame);
        left_mid1_checkbox = new QCheckBox(centralwidget);
        left_mid1_checkbox->setObjectName(QString::fromUtf8("left_mid1_checkbox"));
        left_mid1_checkbox->setGeometry(QRect(100, 280, 16, 20));
        right_mid1_checkbox = new QCheckBox(centralwidget);
        right_mid1_checkbox->setObjectName(QString::fromUtf8("right_mid1_checkbox"));
        right_mid1_checkbox->setGeometry(QRect(200, 280, 16, 20));
        left_right_checkbox = new QCheckBox(centralwidget);
        left_right_checkbox->setObjectName(QString::fromUtf8("left_right_checkbox"));
        left_right_checkbox->setGeometry(QRect(150, 260, 16, 20));
        mid1_mid2_checkbox = new QCheckBox(centralwidget);
        mid1_mid2_checkbox->setObjectName(QString::fromUtf8("mid1_mid2_checkbox"));
        mid1_mid2_checkbox->setGeometry(QRect(170, 300, 16, 20));
        mid2_mid3_checkbox = new QCheckBox(centralwidget);
        mid2_mid3_checkbox->setObjectName(QString::fromUtf8("mid2_mid3_checkbox"));
        mid2_mid3_checkbox->setGeometry(QRect(170, 350, 16, 20));
        right_mid2_checkbox = new QCheckBox(centralwidget);
        right_mid2_checkbox->setObjectName(QString::fromUtf8("right_mid2_checkbox"));
        right_mid2_checkbox->setGeometry(QRect(200, 310, 16, 20));
        left_mid2_checkbox = new QCheckBox(centralwidget);
        left_mid2_checkbox->setObjectName(QString::fromUtf8("left_mid2_checkbox"));
        left_mid2_checkbox->setGeometry(QRect(100, 310, 16, 20));
        mid1_mid3_checkbox = new QCheckBox(centralwidget);
        mid1_mid3_checkbox->setObjectName(QString::fromUtf8("mid1_mid3_checkbox"));
        mid1_mid3_checkbox->setGeometry(QRect(130, 350, 16, 20));
        left_mid3_checkbox = new QCheckBox(centralwidget);
        left_mid3_checkbox->setObjectName(QString::fromUtf8("left_mid3_checkbox"));
        left_mid3_checkbox->setGeometry(QRect(90, 330, 16, 20));
        right_mid3_checkbox = new QCheckBox(centralwidget);
        right_mid3_checkbox->setObjectName(QString::fromUtf8("right_mid3_checkbox"));
        right_mid3_checkbox->setGeometry(QRect(220, 330, 16, 20));
        color_stream_widget = new CustomOpenGLWidget(centralwidget);
        color_stream_widget->setObjectName(QString::fromUtf8("color_stream_widget"));
        color_stream_widget->setGeometry(QRect(420, 20, 401, 301));
        depth_stream_widget = new CustomOpenGLWidget(centralwidget);
        depth_stream_widget->setObjectName(QString::fromUtf8("depth_stream_widget"));
        depth_stream_widget->setGeometry(QRect(850, 20, 401, 301));
        volume_stream_widget = new QWidget(centralwidget);
        volume_stream_widget->setObjectName(QString::fromUtf8("volume_stream_widget"));
        volume_stream_widget->setGeometry(QRect(420, 350, 401, 301));
        volume_scatter = new SynchronizedQ3DScatter();
        volume_widget_container = QWidget::createWindowContainer(volume_scatter);
        volume_hLayout = new QHBoxLayout(volume_stream_widget);
        volume_vLayout = new QVBoxLayout();
        volume_hLayout->addWidget(volume_widget_container, 1);
        volume_hLayout->addLayout(volume_vLayout);
        volume_scatter_modifier = new ScatterDataModifier(volume_scatter);

        graph_widget = new QCustomPlot(centralwidget);
        graph_widget->setObjectName(QString::fromUtf8("graph_widget"));
        graph_widget->setGeometry(QRect(850, 350, 401, 301));
		bpm_text = new QLabel(centralwidget);
		bpm_text->setObjectName(QString::fromUtf8("bpm_text"));
		bpm_text->setGeometry(QRect(850, 660, 81, 41));
		QFont font;
		font.setPointSize(12);
		bpm_text->setFont(font);
		bpm_text->setMouseTracking(false);
		bpm_text->setFrameShape(QFrame::NoFrame);
		bpm_value = new QLabel(centralwidget);
		bpm_value->setObjectName(QString::fromUtf8("bpm_value"));
		bpm_value->setGeometry(QRect(950, 660, 61, 41));
		bpm_value->setFont(font);
		bpm_value->setMouseTracking(false);
		bpm_value->setFrameShape(QFrame::NoFrame);
        fps_text = new QLabel(centralwidget);
        fps_text->setObjectName(QString::fromUtf8("fps_text"));
        fps_text->setGeometry(QRect(1080, 660, 81, 41));
        fps_text->setFont(font);
        fps_text->setMouseTracking(false);
        fps_text->setFrameShape(QFrame::NoFrame);
		fps_value = new QLabel(centralwidget);
        fps_value->setObjectName(QString::fromUtf8("fps_value"));
        fps_value->setGeometry(QRect(1170, 660, 61, 41));
        fps_value->setFont(font);
        fps_value->setMouseTracking(false);
        fps_value->setFrameShape(QFrame::NoFrame);
        DeepBreath->setCentralWidget(centralwidget);
        menubar = new QMenuBar(DeepBreath);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 1280, 26));
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
        dimension_text->setText(QCoreApplication::translate("DeepBreath", "Dimension:", nullptr));
        mode_text->setText(QCoreApplication::translate("DeepBreath", "Mode:", nullptr));
        mode_combo_box->setItemText(0, QCoreApplication::translate("DeepBreath", "Distances", nullptr));
        mode_combo_box->setItemText(1, QCoreApplication::translate("DeepBreath", "Locations", nullptr));
        mode_combo_box->setItemText(2, QCoreApplication::translate("DeepBreath", "Fourier", nullptr));
        mode_combo_box->setItemText(3, QCoreApplication::translate("DeepBreath", "Volume", nullptr));
        mode_combo_box->setItemText(4, QCoreApplication::translate("DeepBreath", "No Graph", nullptr));
        volume_type_combo_box->setItemText(0, QCoreApplication::translate("DeepBreath", "Tetrahedron", nullptr));
        volume_type_combo_box->setItemText(1, QCoreApplication::translate("DeepBreath", "Reimann Sums", nullptr));
        locations_text->setText(QCoreApplication::translate("DeepBreath", "Locations:", nullptr));
        left_loc_checkbox->setText(QCoreApplication::translate("DeepBreath", "Left", nullptr));
        right_loc_checkbox->setText(QCoreApplication::translate("DeepBreath", "Right", nullptr));
        mid1_loc_checkbox->setText(QCoreApplication::translate("DeepBreath", "mid1", nullptr));
        mid2_loc_checkbox->setText(QCoreApplication::translate("DeepBreath", "mid2", nullptr));
        mid3_loc_checkbox->setText(QCoreApplication::translate("DeepBreath", "mid3", nullptr));
        num_markers_text->setText(QCoreApplication::translate("DeepBreath", "#Markers:", nullptr));
        num_markers_combo_box->setItemText(0, QCoreApplication::translate("DeepBreath", "3", nullptr));
        num_markers_combo_box->setItemText(1, QCoreApplication::translate("DeepBreath", "4", nullptr));
        num_markers_combo_box->setItemText(2, QCoreApplication::translate("DeepBreath", "5", nullptr));

        is_stickers_checkbox->setText(QCoreApplication::translate("DeepBreath", "Stickers", nullptr));
        y_color_radio_button->setText(QCoreApplication::translate("DeepBreath", "Y", nullptr));
        g_color_radio_button->setText(QCoreApplication::translate("DeepBreath", "G", nullptr));
        b_color_radio_button->setText(QCoreApplication::translate("DeepBreath", "B", nullptr));
        distances_text->setText(QCoreApplication::translate("DeepBreath", "Distances:", nullptr));
        left_text->setText(QCoreApplication::translate("DeepBreath", "Left", nullptr));
        mid1_text->setText(QCoreApplication::translate("DeepBreath", "mid1", nullptr));
        right_text->setText(QCoreApplication::translate("DeepBreath", "Right", nullptr));
        mid2_text->setText(QCoreApplication::translate("DeepBreath", "mid2", nullptr));
        mid3_text->setText(QCoreApplication::translate("DeepBreath", "mid3", nullptr));
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
		bpm_text->setText(QCoreApplication::translate("DeepBreath", "BPM:", nullptr));
		bpm_value->setText(QCoreApplication::translate("DeepBreath", "0", nullptr));
        fps_text->setText(QCoreApplication::translate("DeepBreath", "FPS:", nullptr));
        fps_value->setText(QCoreApplication::translate("DeepBreath", "0", nullptr));
    } // retranslateUi

};

namespace Ui {
    class DeepBreath: public Ui_DeepBreath {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DEEPBREATHGUI_H
