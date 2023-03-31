#include "directory_selection.h"

#include <QHBoxLayout>
#include <QVBoxLayout>

#include <QLineEdit>
#include <QPushButton>
#include <QCheckBox>
#include <QProgressBar>
#include <QLabel>

#include <QDirIterator>
#include <QThreadPool>
#include <QImageReader>
#include <QImageWriter>

#include <QDebug>

Directory_Selection::Directory_Selection(MainWindow* parent) : QDialog(parent){
	_parent = parent;
	setLayout(make_face());

	resize(600, 200);
}

#define turn(b) _from->setEnabled(b);\
_to->setEnabled(b);\
_recur->setEnabled(b);\
_from_b->setEnabled(b);\
_to_b->setEnabled(b);

void Directory_Selection::run(){
	auto task = [&](){
		_stop = false;
		turn(false);

		int file_count = 0;
		QDirIterator it0(_from->text(), QDir::Files,  (_recur->isChecked() ? QDirIterator::Subdirectories : QDirIterator::NoIteratorFlags));
		while(!_stop && it0.hasNext()) if(it0.next() > 0) file_count++;

		connect(this, &Directory_Selection::update_bar, _bar, &QProgressBar::setValue);

		int cur_file = 0;
		QDirIterator it(_from->text(), QDir::Files,  (_recur->isChecked() ? QDirIterator::Subdirectories : QDirIterator::NoIteratorFlags));
		while(!_stop && it.hasNext()) {
			auto file_path = it.next();

			QImageReader reader(file_path);
			reader.setAutoTransform(true);
			QImage cur_image = reader.read();
			if (!cur_image.isNull()) {
				_parent->_marker.place_watermark(cur_image, _parent->collect_sliders(cur_image.size()),
												 (float)_parent->_opacity->get_value() / 100);

				auto out_path = (_to->text() > 0 ? _to->text() : _from->text()) + "/" + file_path.split(_from->text() + "/")[1];
				QDir dir(QFileInfo(out_path).path());
				if (!dir.exists()) dir.mkpath(".");

				QImageWriter writer(out_path);
				writer.write(cur_image);
			}

			emit update_bar((float)(cur_file++) / file_count * 100);
		}

		emit update_bar(100);
		turn(true);
	};
	QThreadPool::globalInstance()->start(task);
}

QLayout* Directory_Selection::make_face(){
	auto out = new QVBoxLayout();

	auto from = new QHBoxLayout();
	from->addWidget(new QLabel(tr("from")));
	from->addWidget(_from = new QLineEdit());
	from->addWidget(_from_b = new QPushButton(tr("Choose")));
	connect(_from_b, &QPushButton::clicked, this, [&](){_from->setText(open_dir()); });

	auto to = new QHBoxLayout();
	to->addWidget(new QLabel(tr("to")));
	to->addWidget(_to = new QLineEdit());
	to->addWidget(_to_b = new QPushButton(tr("Choose")));
	connect(_to_b, &QPushButton::clicked, this, [&](){_to->setText(open_dir()); });

	auto bar = new QHBoxLayout();
	bar->addWidget(_recur = new QCheckBox(tr("Recursively")));
	bar->addWidget(_bar = new QProgressBar());
	_bar->setValue(0);

	auto buttons = new QHBoxLayout();
	QPushButton* run_b, *cancel_b;
	buttons->addWidget(run_b = new QPushButton(tr("Run")));
	buttons->addWidget(cancel_b = new QPushButton(tr("Cancel")));
	connect(run_b, &QPushButton::clicked, this, &Directory_Selection::run);
	connect(cancel_b, &QPushButton::clicked, this, &Directory_Selection::cancel);

	out->addLayout(from);
	out->addLayout(to);
	out->addLayout(bar);
	out->addLayout(buttons);

	return out;
}

QString Directory_Selection::open_dir(){
	QFileDialog d; d.setFileMode(QFileDialog::Directory);

	while(d.exec() == QDialog::Accepted) return d.directory().path();
	return QString();
}

void Directory_Selection::cancel(){
	_stop = true;
	this->close();
}
