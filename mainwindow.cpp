#include "mainwindow.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QSlider>
#include <QSpinBox>
#include <QSplitter>
#include <QCheckBox>

#include <QAction>
#include <QMenu>
#include <QMenuBar>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent){
	auto s = new QSplitter(Qt::Orientation::Horizontal);
	auto w = new QWidget;
	w->setLayout(make_slider_area());

	s->addWidget(w);
	_viewer = new Image_Viewer(this);
//	connect(_viewer, &Image_Viewer::image_loaded, this, &MainWindow::update_sliders);
	connect(_viewer, &Image_Viewer::image_seted, this, [&](){fitToWindowAct->setEnabled(true);});
	s->addWidget(_viewer);

	setCentralWidget(s);

	create_actions();
	resize(1200, 600);
}

QLayout* MainWindow::new_lay(const QString& v, Slider_With_Number* slide){
	auto out = new QHBoxLayout();
	out->addWidget(new QLabel(v));
	out->addWidget(slide);
	connect(slide->spin, QOverload<int>::of(&QSpinBox::valueChanged), this, &MainWindow::update_mark);
	return out;
}

QLayout* MainWindow::make_slider_area(){
	auto out = new QVBoxLayout();

	auto pos = new QGridLayout();
	qint8 row = 0;
	pos->addWidget(new QLabel("Position"), row++, 0);
	pos->addLayout(new_lay("x", pos_x = new Slider_With_Number()), row++, 0);
	pos->addLayout(new_lay("y", pos_y = new Slider_With_Number()), row++, 0);
	pos_x->set_min_max(0, 100);
	pos_y->set_min_max(0, 100);

	auto rot = new QGridLayout();
	row = 0;
	rot->addWidget(new QLabel("Rotation"), row++, 0);
	rot->addLayout(new_lay("x", rot_x = new Slider_With_Number()), row++, 0);
//	rot->addLayout(new_lay("y", rot_y = new Slider_With_Number()), row++, 0);
	rot_x->set_min_max(-360, 360);
//	rot_y->set_min_max(0, 360);

	auto scale = new QGridLayout();
	row = 0;
	scale->addWidget(new QLabel("Scale"), row++, 0);
	QCheckBox* cb;
	scale->addWidget(cb = new QCheckBox("Proportionately"), row++, 0);
	scale->addLayout(new_lay("x", scale_x = new Slider_With_Number()), row++, 0);
	scale->addLayout(new_lay("y", scale_y = new Slider_With_Number()), row++, 0);
	scale_x->set_min_max(-200, 200);
	scale_y->set_min_max(-200, 200);
	scale_x->set_value(100);
	scale_y->set_value(100);

	cb->setChecked(true);
	connect(cb, &QCheckBox::stateChanged, this, [&](int c){ _is_scale_prop = c; });
	auto prop = [&](int inp){
		if(!_is_scale_prop){
			int vx = scale_x->get_value(), vy = scale_y->get_value();
			_scale_prop = !vx && !vy ? 0 : (float)vx / vy;
			return;
		}

		if(inp != scale_y->get_value()) scale_y->set_value(scale_x->get_value() / _scale_prop);
		else scale_x->set_value(scale_y->get_value() * _scale_prop);
	};
	connect(scale_x->spin, QOverload<int>::of(&QSpinBox::valueChanged), this, prop);
	connect(scale_y->spin, QOverload<int>::of(&QSpinBox::valueChanged), this, prop);

	auto opacity = new QGridLayout();
	row = 0;
	opacity->addWidget(new QLabel("Opacity"), row++, 0);
	opacity->addLayout(new_lay("", _opacity = new Slider_With_Number()), row++, 0);
	_opacity->set_min_max(0, 100);
	_opacity->spin->setValue(100);

	out->addLayout(pos);
	out->addLayout(rot);
	out->addLayout(scale);
	out->addLayout(opacity);

	return out;
}

void MainWindow::create_actions(){
	QMenu *fileMenu = menuBar()->addMenu(tr("&File"));

	QAction *openAct = fileMenu->addAction(tr("&Open..."), this, [&](){
		_viewer->open();
		updateActions();
	});
	openAct->setShortcut(QKeySequence::Open);

	QAction* load_mark = fileMenu->addAction(tr("&Load mark..."), this, [&](){
		_viewer->clear_image();
		_marker.set_mark(Image_Viewer::load_image());
//		_marker.place_watermark(_viewer->get_image());
		update_mark(1);
		_viewer->update_image();
	});
	load_mark->setShortcut(tr("Ctrl+L"));

	saveAsAct = fileMenu->addAction(tr("&Save As..."), _viewer, &Image_Viewer::saveAs);
	saveAsAct->setEnabled(false);

	fileMenu->addSeparator();

	QAction *exitAct = fileMenu->addAction(tr("&Exit"), _viewer, &QWidget::close);
	exitAct->setShortcut(tr("Ctrl+Q"));

	QMenu *viewMenu = menuBar()->addMenu(tr("&View"));

	zoomInAct = viewMenu->addAction(tr("Zoom &In (25%)"),this, [&](){
		_viewer->zoomIn();
		updateActions();
	});
	zoomInAct->setShortcut(QKeySequence::ZoomIn);
	zoomInAct->setEnabled(false);

	zoomOutAct = viewMenu->addAction(tr("Zoom &Out (25%)"), this, [&](){
		_viewer->zoomOut();
		updateActions();
	});
	zoomOutAct->setShortcut(QKeySequence::ZoomOut);
	zoomOutAct->setEnabled(false);

	normalSizeAct = viewMenu->addAction(tr("&Normal Size"), this, [&](){
		_viewer->normalSize();
		updateActions();
	});
	normalSizeAct->setShortcut(tr("Ctrl+S"));
	normalSizeAct->setEnabled(false);

	viewMenu->addSeparator();

	fitToWindowAct = viewMenu->addAction(tr("&Fit to Window"), this, [&](bool b){
		_viewer->fitToWindow(b);
		updateActions();
	});
	fitToWindowAct->setEnabled(false);
	fitToWindowAct->setCheckable(true);
	fitToWindowAct->setShortcut(tr("Ctrl+F"));
}

inline void MainWindow::updateActions(){
	saveAsAct->setEnabled(_viewer->has_image());
	zoomInAct->setEnabled(!fitToWindowAct->isChecked());
	zoomOutAct->setEnabled(!fitToWindowAct->isChecked());
	normalSizeAct->setEnabled(!fitToWindowAct->isChecked());
}

//void MainWindow::update_sliders(const QImage& image){

//}

void MainWindow::update_mark(int){
	if(_viewer == nullptr || _viewer->get_image().isNull()) return;
	auto s = _viewer->get_image().size();

	QTransform tr;
	tr.translate(s.width() * (float)pos_x->get_value() / 100, s.height() * (float)pos_y->get_value() / 100);
	tr.scale((float)scale_x->get_value() / 100, (float)scale_y->get_value() / 100);
//	tr.rotate(rot_x->get_value());
	QSize center = _marker.get_mark().size();
	tr.translate(center.width() / 2, center.height() / 2)
			.rotate(rot_x->get_value()).translate(-center.width() / 2, -center.height() / 2);

	_viewer->clear_image();
	_marker.place_watermark(_viewer->get_image(), tr, (float)_opacity->get_value() / 100);
	_viewer->update_image();
}


Slider_With_Number::Slider_With_Number(QWidget* parent) :
	Slider_With_Number(new QSlider(Qt::Orientation::Horizontal), new QSpinBox(), parent) {}

Slider_With_Number::Slider_With_Number(QSlider* slide_, QSpinBox* spin_, QWidget* parent) : QWidget(parent)
, slide(slide_), spin(spin_){
	QObject::connect(slide, &QSlider::valueChanged, spin, &QSpinBox::setValue);
	QObject::connect(spin, QOverload<int>::of(&QSpinBox::valueChanged), slide, &QSlider::setValue);

	auto l = new QHBoxLayout();
	l->addWidget(slide);
	l->addWidget(spin);
	setLayout(l);
}

void Slider_With_Number::set_min_max(int min, int max){
	slide->setMinimum(min);
	slide->setMaximum(max);

	spin->setMinimum(min);
	spin->setMaximum(max);
}

void Slider_With_Number::set_value(int val){
	spin->setValue(val);
}

int Slider_With_Number::get_value(){
	return spin->value();
}
