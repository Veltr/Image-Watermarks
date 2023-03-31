#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "image_viewer.h"
#include "marker.h"

#include <QMainWindow>

QT_BEGIN_NAMESPACE
class QLayout;
class QSlider;
class QSpinBox;
QT_END_NAMESPACE

class Slider_With_Number : public QWidget {
public:
	Slider_With_Number(QWidget* parent = nullptr);
	Slider_With_Number(QSlider* slide, QSpinBox* spin, QWidget* parent = nullptr);
	void set_min_max(int min, int max);

	void set_value(int val);
	int get_value();

	QSlider* slide;
	QSpinBox* spin;
};

class MainWindow : public QMainWindow {
	Q_OBJECT

public:
	MainWindow(QWidget *parent = nullptr);

private slots:
//	void update_sliders(const QImage&);
	void update_mark(int);

private:
	Image_Viewer* _viewer = nullptr;
	Marker _marker;

	QAction* saveAsAct;
	QAction* zoomInAct;
	QAction* zoomOutAct;
	QAction* normalSizeAct;
	QAction* fitToWindowAct;

	void create_actions();
	inline void updateActions();

	Slider_With_Number* pos_x;
	Slider_With_Number* pos_y;
	Slider_With_Number* rot_x;
//	Slider_With_Number* rot_y;
	Slider_With_Number* scale_x;
	Slider_With_Number* scale_y;

	Slider_With_Number* _opacity;

	QLayout* make_slider_area();
	QLayout* new_lay(const QString& v, Slider_With_Number* slide);
};

#endif // MAINWINDOW_H

