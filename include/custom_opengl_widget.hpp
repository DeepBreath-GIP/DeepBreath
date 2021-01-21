#pragma once

#include <QOpenGLWidget>
#include <QImage>
#include <QPainter>
#include <QPaintEvent>

class CustomOpenGLWidget : public QOpenGLWidget {
private:
	QImage image;
	QImage black_image;

public:
	CustomOpenGLWidget(QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());

	void display(const QImage& img);

	void clear();

protected:
	void paintEvent(QPaintEvent* e);
};