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
	void update_mark(int);
	bool load_preset(const QString&);
	bool save_preset(const QString&);

private:
	Image_Viewer* _viewer = nullptr;
	Marker _marker;

	QAction* _save_as_act;
	QAction* _zoom_in_act;
	QAction* _zoom_out_act;
	QAction* _normal_size_act;
	QAction* fit_to_window_act;

	QAction* _preset_save_act;
	QAction* _preset_load_act;

	void create_actions();
	inline void update_actions();

	Slider_With_Number* _pos_x;
	Slider_With_Number* _pos_y;
	Slider_With_Number* _rot_x;
	Slider_With_Number* _scale_x;
	Slider_With_Number* _scale_y;
	bool _is_scale_prop = true;
	float _scale_prop = 1.;
	bool _one_scaled = false;

	Slider_With_Number* _opacity;

	QLayout* make_slider_area();
	QLayout* new_lay(const QString& v, Slider_With_Number* slide);

	QTransform collect_sliders(const QSize& size);

	friend class Directory_Selection;
};

#endif // MAINWINDOW_H

