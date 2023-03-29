#include "mainwindow.h"

#include <QVBoxLayout>
#include <QHBoxLayout>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent){
	QWidget* w = new QWidget;

	w->setLayout(make_face());
	setCentralWidget(w);
}

MainWindow::~MainWindow(){

}

QLayout* MainWindow::make_face(){

}

