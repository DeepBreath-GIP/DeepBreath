#include "custom_opengl_widget.hpp"

CustomOpenGLWidget::CustomOpenGLWidget(QWidget* parent, Qt::WindowFlags f) : QOpenGLWidget(parent, f | Qt::WindowType::MSWindowsOwnDC), image(), black_image(800, 800, QImage::Format_Mono) {
	QPainter pnt;
	pnt.begin(&black_image);
	pnt.fillRect(0, 0, 800, 800, Qt::black);
}

void CustomOpenGLWidget::display(const QImage& img)
{
	image = img;
	this->update();
}

void CustomOpenGLWidget::clear()
{
	// Paint black image to clear the picture
	display(black_image);
}

void CustomOpenGLWidget::paintEvent(QPaintEvent* e)
{
	QPainter painter(this);
	painter.drawImage(e->rect(), image);
	painter.end();
}
