#pragma once

#include <QOpenGLWidget>
#include <QImage>
#include <QPainter>

class CustomOpenGLWidget : public QOpenGLWidget {
private:
	QImage image;

public:

	CustomOpenGLWidget(QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags()) : QOpenGLWidget(parent, f) {}

	void display(const QImage& img)
	{
		image = img;
		this->update();
	}

	void paintEvent(QPaintEvent* e)
	{
		QPainter painter(this);
		painter.drawImage(this->rect(), image);
		// According to QOpenGLWidget description we need to make sure that OpenGL framebuffer object bound in the context
		// https://doc.qt.io/qt-5/qopenglwidget.html#:~:text=If%20you%20need%20to%20trigger,function%20to%20schedule%20an%20update.
		this->makeCurrent();
	}

	void clear() {
		// Paint black image to clear the picture
		QImage black_image(800, 800, QImage::Format_Mono);
		QPainter pnt;
		pnt.begin(&black_image);
		pnt.fillRect(0, 0, 800, 800, Qt::black);
		display(black_image);
	}
};