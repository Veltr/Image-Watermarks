#ifndef IMAGE_VIEWER_H
#define IMAGE_VIEWER_H

#include <QMainWindow>
#include <QScrollArea>
#include <QImage>
#include <QFileDialog>

QT_BEGIN_NAMESPACE
class QAction;
class QLabel;
class QMenu;
class QScrollArea;
class QScrollBar;
QT_END_NAMESPACE

class Image_Viewer : public QScrollArea {
	Q_OBJECT

public:
	Image_Viewer(QWidget *parent = nullptr);
	bool loadFile(const QString &);
	void setImage(const QImage &newImage);

	static void init_image_FileDialog(QFileDialog &dialog, QFileDialog::AcceptMode acceptMode);

signals:
	void image_loaded(const QImage&);

private slots:
	void open();
	void saveAs();
	void zoomIn();
	void zoomOut();
	void normalSize();
	void fitToWindow();

private:
	QImage image;
	QLabel *imageLabel;
	double scaleFactor = 1;

	void scaleImage(double factor);
	void adjustScrollBar(QScrollBar *scrollBar, double factor);
	bool saveFile(const QString &fileName);

	void createActions(QMenuBar* menuBar);
	inline void updateActions();

	bool has_actions = false;
	QAction* saveAsAct;
	QAction* zoomInAct;
	QAction* zoomOutAct;
	QAction* normalSizeAct;
	QAction* fitToWindowAct;
};

#endif // IMAGE_VIEWER_H
