#include "scatterdatamodifier.hpp"
#include <QtDataVisualization/qscatterdataproxy.h>
#include <QtDataVisualization/qvalue3daxis.h>
#include <QtDataVisualization/q3dscene.h>
#include <QtDataVisualization/q3dcamera.h>
#include <QtDataVisualization/qscatter3dseries.h>
#include <QtDataVisualization/q3dtheme.h>
#include <QtCore/qmath.h>
#include <QtCore/qrandom.h>
#include <QtWidgets/QComboBox>

using namespace QtDataVisualization;

ScatterDataModifier::ScatterDataModifier(SynchronizedQ3DScatter* scatter)
    : m_graph(scatter),
    m_fontSize(40.0f),
    m_style(QAbstract3DSeries::MeshSphere),
    m_smooth(true)
{
    m_graph->activeTheme()->setType(Q3DTheme::ThemeEbony);
    QFont font = m_graph->activeTheme()->font();
    font.setPointSize(m_fontSize);
    m_graph->activeTheme()->setFont(font);
    m_graph->setShadowQuality(QAbstract3DGraph::ShadowQualitySoftLow);
    m_graph->scene()->activeCamera()->setCameraPreset(Q3DCamera::CameraPresetFront);

    QScatterDataProxy* proxy = new QScatterDataProxy;
    QScatter3DSeries* series = new QScatter3DSeries(proxy);
    series->setItemLabelFormat(QStringLiteral("@xTitle: @xLabel @yTitle: @yLabel @zTitle: @zLabel"));
    series->setMeshSmooth(m_smooth);
    m_graph->addSeries(series);

    // Configure the axes according to the data
    m_graph->axisX()->setTitle("X");
    m_graph->axisY()->setTitle("Y");
    m_graph->axisZ()->setTitle("Z");
}

ScatterDataModifier::~ScatterDataModifier()
{
    delete m_graph;
}

void ScatterDataModifier::addData(cv::Vec3f** points, int w, int h)
{
    QScatterDataArray* dataArray = new QScatterDataArray(h * w);
    QScatterDataItem* ptrToDataArray = &dataArray->first();


    for (int i = 0; i < h; i++) {
        for (int j = 0; j < w; j++) {
            ptrToDataArray->setPosition(QVector3D(points[i][j][0], points[i][j][1], points[i][j][2]));
            ptrToDataArray++;
        }
    }

    std::unique_lock<std::mutex> lk(m_graph->get_sync_event_mutex());
    m_graph->seriesList().at(0)->dataProxy()->resetArray(dataArray);
}

void ScatterDataModifier::changeStyle(int style)
{
    QComboBox* comboBox = qobject_cast<QComboBox*>(sender());
    if (comboBox) {
        m_style = QAbstract3DSeries::Mesh(comboBox->itemData(style).toInt());
        if (m_graph->seriesList().size())
            m_graph->seriesList().at(0)->setMesh(m_style);
    }
}

void ScatterDataModifier::setSmoothDots(int smooth)
{
    m_smooth = bool(smooth);
    QScatter3DSeries* series = m_graph->seriesList().at(0);
    series->setMeshSmooth(m_smooth);
}

void ScatterDataModifier::changeTheme(int theme)
{
    Q3DTheme* currentTheme = m_graph->activeTheme();
    currentTheme->setType(Q3DTheme::Theme(theme));
    emit backgroundEnabledChanged(currentTheme->isBackgroundEnabled());
    emit gridEnabledChanged(currentTheme->isGridEnabled());
    emit fontChanged(currentTheme->font());
}

void ScatterDataModifier::changePresetCamera()
{
    static int preset = Q3DCamera::CameraPresetFrontLow;

    m_graph->scene()->activeCamera()->setCameraPreset((Q3DCamera::CameraPreset)preset);

    if (++preset > Q3DCamera::CameraPresetDirectlyBelow)
        preset = Q3DCamera::CameraPresetFrontLow;
}

void ScatterDataModifier::changeLabelStyle()
{
    m_graph->activeTheme()->setLabelBackgroundEnabled(!m_graph->activeTheme()->isLabelBackgroundEnabled());
}

void ScatterDataModifier::changeFont(const QFont& font)
{
    QFont newFont = font;
    newFont.setPointSizeF(m_fontSize);
    m_graph->activeTheme()->setFont(newFont);
}

void ScatterDataModifier::shadowQualityUpdatedByVisual(QAbstract3DGraph::ShadowQuality sq)
{
    int quality = int(sq);
    emit shadowQualityChanged(quality); // connected to a checkbox in main.cpp
}

void ScatterDataModifier::changeShadowQuality(int quality)
{
    QAbstract3DGraph::ShadowQuality sq = QAbstract3DGraph::ShadowQuality(quality);
    m_graph->setShadowQuality(sq);
}

void ScatterDataModifier::setBackgroundEnabled(int enabled)
{
    m_graph->activeTheme()->setBackgroundEnabled((bool)enabled);
}

void ScatterDataModifier::setGridEnabled(int enabled)
{
    m_graph->activeTheme()->setGridEnabled((bool)enabled);
}

void ScatterDataModifier::clear()
{
    std::unique_lock<std::mutex> lk(m_graph->get_sync_event_mutex());
    m_graph->seriesList().at(0)->dataProxy()->resetArray(nullptr);
}

