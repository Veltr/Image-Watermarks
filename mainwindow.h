#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
class QLayout;
class QSlider;
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
	Q_OBJECT

public:
	MainWindow(QWidget *parent = nullptr);

private slots:
	void update_sliders(const QImage&);

private:
	QSlider* pos_x;
	QSlider* pos_y;
	QSlider* rot_x;
	QSlider* rot_y;
	QSlider* scale_x;
	QSlider* scale_y;

	QLayout* make_slider_area();
};
#endif // MAINWINDOW_H

