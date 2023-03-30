#include "mainwindow.h"
#include "image_viewer.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QSlider>
#include <QSpinBox>
#include <QSplitter>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent){
	auto s = new QSplitter(Qt::Orientation::Horizontal);
	auto w = new QWidget;
	w->setLayout(make_slider_area());

	s->addWidget(w);
	auto viewer = new Image_Viewer(this);
	connect(viewer, &Image_Viewer::image_loaded, this, &MainWindow::update_sliders);
	s->addWidget(viewer);

	setCentralWidget(s);

	resize(1200, 600);
}

inline QLayout* new_lay(const QString& v, QSlider* bar){
	auto out = new QHBoxLayout();
	out->addWidget(new QLabel(v));
	out->addWidget(bar);
	auto spin = new QSpinBox();
	out->addWidget(spin);
	QObject::connect(bar, &QSlider::valueChanged, spin, &QSpinBox::setValue);
	QObject::connect(spin, QOverload<int>::of(&QSpinBox::valueChanged), bar, &QSlider::setValue);

	return out;
}

QLayout* MainWindow::make_slider_area(){
	auto out = new QVBoxLayout();

	auto pos = new QGridLayout();
	qint8 row = 0;
	pos->addWidget(new QLabel("Position"), row++, 0);
	pos->addLayout(new_lay("x", pos_x = new QSlider(Qt::Orientation::Horizontal)), row++, 0);
	pos->addLayout(new_lay("y", pos_y = new QSlider(Qt::Orientation::Horizontal)), row++, 0);

	auto rot = new QGridLayout();
	row = 0;
	rot->addWidget(new QLabel("Rotation"), row++, 0);
	rot->addLayout(new_lay("x", rot_x = new QSlider(Qt::Orientation::Horizontal)), row++, 0);
	rot->addLayout(new_lay("y", rot_y = new QSlider(Qt::Orientation::Horizontal)), row++, 0);

	auto scale = new QGridLayout();
	row = 0;
	scale->addWidget(new QLabel("Scale"), row++, 0);
	scale->addLayout(new_lay("x", scale_x = new QSlider(Qt::Orientation::Horizontal)), row++, 0);
	scale->addLayout(new_lay("y", scale_y = new QSlider(Qt::Orientation::Horizontal)), row++, 0);

	out->addLayout(pos);
	out->addLayout(rot);
	out->addLayout(scale);

	return out;
}


void MainWindow::update_sliders(const QImage& image){

}


