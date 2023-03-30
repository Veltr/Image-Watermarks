#include "image_viewer.h"

#include <QApplication>
#include <QColorSpace>
#include <QFileDialog>
#include <QImageReader>
#include <QImageWriter>
#include <QLabel>
#include <QMenuBar>
#include <QScrollArea>
#include <QScrollBar>
#include <QStandardPaths>
#include <QMessageBox>
#include <QPixmap>

Image_Viewer::Image_Viewer(QWidget* parent) : QScrollArea(parent)
	, imageLabel(new QLabel) {

	imageLabel->setBackgroundRole(QPalette::Base);
	imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
	imageLabel->setScaledContents(true);

	setBackgroundRole(QPalette::Dark);
	setWidget(imageLabel);

	if(auto p = dynamic_cast<QMainWindow*>(parent)) createActions(p->menuBar());

	QPixmap map(640, 480);
	map.fill();
	setImage(map.toImage());
	emit image_loaded(image);
}

void Image_Viewer::setImage(const QImage &newImage){
	image = newImage;
	if (image.colorSpace().isValid()) image.convertToColorSpace(QColorSpace::SRgb);
	imageLabel->setPixmap(QPixmap::fromImage(image));

	scaleFactor = 1.0;

	setVisible(true);
	fitToWindowAct->setEnabled(true);
	updateActions();

	if (!fitToWindowAct->isChecked()) imageLabel->adjustSize();
}

void Image_Viewer::init_image_FileDialog(QFileDialog &dialog, QFileDialog::AcceptMode acceptMode){
	static bool firstDialog = true;

	if (firstDialog) {
		firstDialog = false;
		const QStringList picturesLocations = QStandardPaths::standardLocations(QStandardPaths::PicturesLocation);
		dialog.setDirectory(picturesLocations.isEmpty() ? QDir::currentPath() : picturesLocations.last());
	}

	QStringList mimeTypeFilters;
	const QByteArrayList supportedMimeTypes = acceptMode == QFileDialog::AcceptOpen
		? QImageReader::supportedMimeTypes() : QImageWriter::supportedMimeTypes();
	for (const QByteArray &mimeTypeName : supportedMimeTypes)
		mimeTypeFilters.append(mimeTypeName);
	mimeTypeFilters.sort();
	dialog.setMimeTypeFilters(mimeTypeFilters);
	dialog.selectMimeTypeFilter("image/jpeg");
	if (acceptMode == QFileDialog::AcceptSave)
		dialog.setDefaultSuffix("jpg");
}

void Image_Viewer::open(){
	QFileDialog dialog(this, tr("Open File"));
	init_image_FileDialog(dialog, QFileDialog::AcceptOpen);

	while (dialog.exec() == QDialog::Accepted && !loadFile(dialog.selectedFiles().first())) {}
}

void Image_Viewer::saveAs(){
	QFileDialog dialog(this, tr("Save File As"));
	init_image_FileDialog(dialog, QFileDialog::AcceptSave);

	while (dialog.exec() == QDialog::Accepted && !saveFile(dialog.selectedFiles().first())) {}
}

void Image_Viewer::zoomIn(){
	scaleImage(1.25);
}

void Image_Viewer::zoomOut(){
	scaleImage(0.8);
}

void Image_Viewer::normalSize(){
	imageLabel->adjustSize();
	scaleFactor = 1.0;
}

void Image_Viewer::fitToWindow(){
	bool fitToWindow = fitToWindowAct->isChecked();
	setWidgetResizable(fitToWindow);
	if (!fitToWindow)
		normalSize();
	updateActions();
}

void Image_Viewer::scaleImage(double factor){
	scaleFactor *= factor;
	imageLabel->resize(scaleFactor * imageLabel->pixmap(Qt::ReturnByValue).size());

	adjustScrollBar(horizontalScrollBar(), factor);
	adjustScrollBar(verticalScrollBar(), factor);

	zoomInAct->setEnabled(scaleFactor < 3.0);
	zoomOutAct->setEnabled(scaleFactor > 0.333);
}

void Image_Viewer::adjustScrollBar(QScrollBar *scrollBar, double factor){
	scrollBar->setValue(int(factor * scrollBar->value()
							+ ((factor - 1) * scrollBar->pageStep()/2)));
}

bool Image_Viewer::loadFile(const QString &fileName){
	QImageReader reader(fileName);
	reader.setAutoTransform(true);
	const QImage newImage = reader.read();
	if (newImage.isNull()) {
		QMessageBox::information(this, QGuiApplication::applicationDisplayName(),
								 tr("Cannot load %1: %2")
								 .arg(QDir::toNativeSeparators(fileName), reader.errorString()));
		return false;
	}

	setImage(newImage);
	setWindowFilePath(fileName);

	emit image_loaded(newImage);
	return true;
}

bool Image_Viewer::saveFile(const QString &fileName){
	QImageWriter writer(fileName);

	if (!writer.write(image)) {
		QMessageBox::information(this, QGuiApplication::applicationDisplayName(),
								 tr("Cannot write %1: %2")
								 .arg(QDir::toNativeSeparators(fileName)), writer.errorString());
		return false;
	}

	return true;
}


void Image_Viewer::createActions(QMenuBar* menuBar){
	QMenu *fileMenu = menuBar->addMenu(tr("&File"));

	QAction *openAct = fileMenu->addAction(tr("&Open..."), this, &Image_Viewer::open);
	openAct->setShortcut(QKeySequence::Open);

	saveAsAct = fileMenu->addAction(tr("&Save As..."), this, &Image_Viewer::saveAs);
	saveAsAct->setEnabled(false);

	fileMenu->addSeparator();

	QAction *exitAct = fileMenu->addAction(tr("&Exit"), this, &QWidget::close);
	exitAct->setShortcut(tr("Ctrl+Q"));

	QMenu *viewMenu = menuBar->addMenu(tr("&View"));

	zoomInAct = viewMenu->addAction(tr("Zoom &In (25%)"), this, &Image_Viewer::zoomIn);
	zoomInAct->setShortcut(QKeySequence::ZoomIn);
	zoomInAct->setEnabled(false);

	zoomOutAct = viewMenu->addAction(tr("Zoom &Out (25%)"), this, &Image_Viewer::zoomOut);
	zoomOutAct->setShortcut(QKeySequence::ZoomOut);
	zoomOutAct->setEnabled(false);

	normalSizeAct = viewMenu->addAction(tr("&Normal Size"), this, &Image_Viewer::normalSize);
	normalSizeAct->setShortcut(tr("Ctrl+S"));
	normalSizeAct->setEnabled(false);

	viewMenu->addSeparator();

	fitToWindowAct = viewMenu->addAction(tr("&Fit to Window"), this, &Image_Viewer::fitToWindow);
	fitToWindowAct->setEnabled(false);
	fitToWindowAct->setCheckable(true);
	fitToWindowAct->setShortcut(tr("Ctrl+F"));

	has_actions = true;
}

inline void Image_Viewer::updateActions(){
	if(!has_actions) return;
	saveAsAct->setEnabled(!image.isNull());
	zoomInAct->setEnabled(!fitToWindowAct->isChecked());
	zoomOutAct->setEnabled(!fitToWindowAct->isChecked());
	normalSizeAct->setEnabled(!fitToWindowAct->isChecked());
}

