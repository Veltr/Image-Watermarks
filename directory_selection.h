#ifndef DIRECTORY_SELECTION_H
#define DIRECTORY_SELECTION_H

#include "mainwindow.h"

#include <QDialog>

QT_BEGIN_NAMESPACE
class QLineEdit;
class QProgressBar;
class QCheckBox;
class QPushButton;
QT_END_NAMESPACE

class Directory_Selection : public QDialog {
	Q_OBJECT
public:
	Directory_Selection(MainWindow* parent);

signals:
	void update_bar(int);

private slots:
	void run();
	void cancel();

private:
	MainWindow* _parent;
	QLineEdit* _from;
	QLineEdit* _to;
	QProgressBar* _bar;
	QCheckBox* _recur;
	QPushButton* _from_b;
	QPushButton* _to_b;

	bool _stop = false;

	QLayout* make_face();
	QString open_dir();
};

#endif // DIRECTORY_SELECTION_H
