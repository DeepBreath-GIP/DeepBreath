#pragma once

#include <QOpenGLWidget>
#include <QImage>
#include <QPainter>

class CustomOpenGLWidget : public QOpenGLWidget {
private:
	QImage image;
	QImage black_image;
public:
	CustomOpenGLWidget(QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags()) : QOpenGLWidget(parent, f), image(), black_image(800, 800, QImage::Format_Mono) {
		QPainter pnt;
		pnt.begin(&black_image);
		pnt.fillRect(0, 0, 800, 800, Qt::black);
	}

	void display(const QImage& img)
	{
		image = img;
		this->update();
	}

	void clear() {
		// Paint black image to clear the picture
		display(black_image);
	}

protected:
	virtual void paintGL() {
		QPainter painter(this);
		painter.drawImage(this->rect(), image);
	}
};