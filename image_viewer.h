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
	inline bool has_image(){ return !image.isNull(); }
	inline QImage& get_image(){ return image; }

	static void init_image_FileDialog(QFileDialog &dialog, QFileDialog::AcceptMode acceptMode);
	static QImage load_image(QWidget* parent = nullptr);

signals:
	void image_loaded(const QImage&);
	void image_seted();

public slots:
	void open();
	void saveAs();
	void zoomIn();
	void zoomOut();
	void normalSize();
	void fitToWindow(bool fitToWindow);

	void update_image();
	void clear_image();

private:
	QImage _buf_image;
	QImage image;
	QLabel *imageLabel;
	double scaleFactor = 1;

	void scaleImage(double factor);
	void adjustScrollBar(QScrollBar *scrollBar, double factor);
	bool saveFile(const QString &fileName);
};

#endif // IMAGE_VIEWER_H
