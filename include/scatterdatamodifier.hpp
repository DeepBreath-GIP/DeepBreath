#pragma once

#include <QtDataVisualization/qabstract3dseries.h>
#include <QtGui/QFont>
#include <opencv2/opencv.hpp>
#include "synchronized_q3dscatter.hpp"


using namespace QtDataVisualization;

class ScatterDataModifier : public QObject
{
    Q_OBJECT
public:
    explicit ScatterDataModifier(SynchronizedQ3DScatter* scatter);
    ~ScatterDataModifier();

    void addData(cv::Vec3f** points, int w, int h);
    void changeStyle();
    void changePresetCamera();
    void changeLabelStyle();
    void changeFont(const QFont& font);
    void changeFontSize(int fontsize);
    void setBackgroundEnabled(int enabled);
    void setGridEnabled(int enabled);
    void setSmoothDots(int smooth);
    void clear();
    void start();

public Q_SLOTS:
    void changeStyle(int style);
    void changeTheme(int theme);
    void changeShadowQuality(int quality);
    void shadowQualityUpdatedByVisual(QAbstract3DGraph::ShadowQuality shadowQuality);

Q_SIGNALS:
    void backgroundEnabledChanged(bool enabled);
    void gridEnabledChanged(bool enabled);
    void shadowQualityChanged(int quality);
    void fontChanged(QFont font);

private:
    SynchronizedQ3DScatter* m_graph;
    int m_fontSize;
    QAbstract3DSeries::Mesh m_style;
    bool m_smooth;
};