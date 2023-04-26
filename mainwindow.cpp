#include "mainwindow.h"
#include "directory_selection.h"

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
	connect(_viewer, &Image_Viewer::image_seted, this, [&](){fit_to_window_act->setEnabled(true);});
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
	pos->addLayout(new_lay("x", _pos_x = new Slider_With_Number()), row++, 0);
	pos->addLayout(new_lay("y", _pos_y = new Slider_With_Number()), row++, 0);
	_pos_x->set_min_max(0, 100);
	_pos_y->set_min_max(0, 100);

	auto rot = new QGridLayout();
	row = 0;
	rot->addWidget(new QLabel("Rotation"), row++, 0);
	rot->addLayout(new_lay("x", _rot_x = new Slider_With_Number()), row++, 0);
//	rot->addLayout(new_lay("y", rot_y = new Slider_With_Number()), row++, 0);
	_rot_x->set_min_max(-360, 360);
//	rot_y->set_min_max(0, 360);

	auto scale = new QGridLayout();
	row = 0;
	scale->addWidget(new QLabel("Scale"), row++, 0);
	QCheckBox* cb;
	scale->addWidget(cb = new QCheckBox("Proportionately"), row++, 0);
	scale->addLayout(new_lay("x", _scale_x = new Slider_With_Number()), row++, 0);
	scale->addLayout(new_lay("y", _scale_y = new Slider_With_Number()), row++, 0);
	_scale_x->set_min_max(-200, 200);
	_scale_y->set_min_max(-200, 200);
	_scale_x->set_value(100);
	_scale_y->set_value(100);

	cb->setChecked(true);
	connect(cb, &QCheckBox::stateChanged, this, [&](int c){ _is_scale_prop = c; });
	auto prop = [&](int inp){
		if(!_is_scale_prop){
			int vx = _scale_x->get_value(), vy = _scale_y->get_value();
			_scale_prop = !vx && !vy ? 0 : (float)vx / vy;
			return;
		}

		if (_one_scaled) { _one_scaled = false; return; }
		_one_scaled = true;
		if(inp != _scale_y->get_value()) _scale_y->set_value(_scale_x->get_value() / _scale_prop);
		else _scale_x->set_value(_scale_y->get_value() * _scale_prop);
	};
	connect(_scale_x->spin, QOverload<int>::of(&QSpinBox::valueChanged), this, prop);
	connect(_scale_y->spin, QOverload<int>::of(&QSpinBox::valueChanged), this, prop);

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
		update_actions();
	});
	openAct->setShortcut(QKeySequence::Open);

	QAction* load_mark = fileMenu->addAction(tr("&Load mark..."), this, [&](){
		const QImage& loaded = Image_Viewer::load_image();
		if(loaded.isNull()) return;
		_viewer->clear_image();
		_marker.set_mark(loaded);
//		_marker.place_watermark(_viewer->get_image());
		update_mark(1);
		_viewer->update_image();
		update_actions();
	});
	load_mark->setShortcut(tr("Ctrl+L"));

	QAction* open_dir = fileMenu->addAction(tr("&Open directory..."), this, [&](){
		auto dir_s = new Directory_Selection(this);
		dir_s->show();
	});
	open_dir->setShortcut(tr("Ctrl+Shift+O"));

	_save_as_act = fileMenu->addAction(tr("&Save picture As..."), _viewer, &Image_Viewer::saveAs);
	_save_as_act->setEnabled(false);

	fileMenu->addSeparator();

	QAction *exitAct = fileMenu->addAction(tr("&Exit"), _viewer, &QWidget::close);
	exitAct->setShortcut(tr("Ctrl+Q"));

	QMenu *viewMenu = menuBar()->addMenu(tr("&View"));

	_zoom_in_act = viewMenu->addAction(tr("Zoom &In (25%)"),this, [&](){
		_viewer->zoomIn();
		update_actions();
	});
	_zoom_in_act->setShortcut(QKeySequence::ZoomIn);
	_zoom_in_act->setEnabled(false);

	_zoom_out_act = viewMenu->addAction(tr("Zoom &Out (25%)"), this, [&](){
		_viewer->zoomOut();
		update_actions();
	});
	_zoom_out_act->setShortcut(QKeySequence::ZoomOut);
	_zoom_out_act->setEnabled(false);

	_normal_size_act = viewMenu->addAction(tr("&Normal Size"), this, [&](){
		_viewer->normalSize();
		update_actions();
	});
	_normal_size_act->setShortcut(tr("Ctrl+S"));
	_normal_size_act->setEnabled(false);

	viewMenu->addSeparator();

	fit_to_window_act = viewMenu->addAction(tr("&Fit to Window"), this, [&](bool b){
		_viewer->fitToWindow(b);
		update_actions();
	});
	fit_to_window_act->setEnabled(false);
	fit_to_window_act->setCheckable(true);
	fit_to_window_act->setShortcut(tr("Ctrl+F"));

	QMenu* preset_menu = menuBar()->addMenu(tr("&Preset"));

	_preset_load_act = preset_menu->addAction(tr("&Open preset..."), this, [&](){
		QFileDialog dialog(this, tr("Open preset"), QString(), "Preset file (*.ipres)");
		dialog.setDefaultSuffix(".ipres");
		dialog.setAcceptMode(QFileDialog::AcceptOpen);

		while (dialog.exec() == QDialog::Accepted && !load_preset(dialog.selectedFiles().first())) {}
		update_actions();
	});

	_preset_save_act = preset_menu->addAction(tr("&Save preset..."), this, [&](){
		QFileDialog dialog(this, tr("Open preset"), QString(), "Preset file (*.ipres)");
		dialog.setDefaultSuffix(".ipres");
		dialog.setAcceptMode(QFileDialog::AcceptSave);
		while (dialog.exec() == QDialog::Accepted && !save_preset(dialog.selectedFiles().first())) {}
		update_actions();
	});
	_preset_save_act->setEnabled(false);
}

inline void MainWindow::update_actions(){
	_save_as_act->setEnabled(_viewer->has_image());
	_zoom_in_act->setEnabled(!fit_to_window_act->isChecked());
	_zoom_out_act->setEnabled(!fit_to_window_act->isChecked());
	_normal_size_act->setEnabled(!fit_to_window_act->isChecked());
	_preset_save_act->setEnabled(_marker.has_mark());
}

//void MainWindow::update_sliders(const QImage& image){

//}

QTransform MainWindow::collect_sliders(const QSize& s){
	QTransform tr;
	tr.translate(s.width() * (float)_pos_x->get_value() / 100, s.height() * (float)_pos_y->get_value() / 100);
	tr.scale((float)_scale_x->get_value() / 100, (float)_scale_y->get_value() / 100);
//	tr.rotate(rot_x->get_value());
	QSize center = _marker.get_mark().size();
	tr.translate(center.width() / 2, center.height() / 2)
			.rotate(_rot_x->get_value()).translate(-center.width() / 2, -center.height() / 2);

	return tr;
}


void MainWindow::update_mark(int){
	if(_viewer == nullptr || !_marker.has_mark() || _viewer->get_image().isNull()) return;
	auto s = _viewer->get_image().size();

	_viewer->clear_image();
	_marker.place_watermark(_viewer->get_image(), collect_sliders(s), (float)_opacity->get_value() / 100);
	_viewer->update_image();
}

bool MainWindow::load_preset(const QString& filename){
	QFile file(filename);
	if(file.open(QIODevice::ReadOnly)){
		QDataStream in(&file);
		int t;
		in >> t; _pos_x->set_value(t);
		in >> t; _pos_y->set_value(t);
		in >> t; _rot_x->set_value(t);
		in >> t; _scale_x->set_value(t);
		in >> t; _scale_y->set_value(t);
		in >> t; _opacity->set_value(t);

		QImage im; in >> im;
		_marker.set_mark(im);

		update_mark(0);
		return true;
	}
	return false;
}

bool MainWindow::save_preset(const QString& filename){
	QFile file(filename);
	if(file.open(QIODevice::WriteOnly)){
		QDataStream out(&file);
		out << _pos_x->get_value() << _pos_y->get_value() << _rot_x->get_value()
			<< _scale_x->get_value() << _scale_y->get_value() << _opacity->get_value() << _marker.get_mark();
		return true;
	}
	return false;
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
