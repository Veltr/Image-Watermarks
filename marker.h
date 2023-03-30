#ifndef MARKER_H
#define MARKER_H

#include <QPixmap>
#include <QTransform>
#include <QPainter>

class Marker {
public:
	Marker();
	Marker(const QPixmap& mark);
	Marker(const QPixmap& mark, const QTransform& transform);

	void place_watermark(QPixmap& target, const QPixmap& mark);
	inline void place_watermark(QPixmap& target);
	inline void place_watermark(QPixmap& target, const QPixmap& mark, const QTransform& transform);
	inline void place_watermark(QPixmap& target, const QTransform& transform);

	inline void set_mark(const QPixmap& mark){ _mark = mark; }

private:
	QPainter _painter;
	QPixmap _mark;
};

#endif // MARKER_H
