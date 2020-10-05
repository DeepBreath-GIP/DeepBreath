#pragma once

#include <QOpenGLWidget>
#include <QImage>
#include <QPainter>

class CustomOpenGLWidget : public QOpenGLWidget {

	QImage image;

public:

	CustomOpenGLWidget(QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags()) : QOpenGLWidget(parent) {}

	void CustomOpenGLWidget::display(const QImage& img)
	{
		image = img;
		this->update();
	}

	void CustomOpenGLWidget::paintEvent(QPaintEvent*)
	{
		QPainter painter(this);
		painter.drawImage(this->rect(), image);
	}
};