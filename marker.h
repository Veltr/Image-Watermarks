#ifndef MARKER_H
#define MARKER_H

#include <QImage>
#include <QPainter>
#include <QTransform>

class Marker {
public:
	Marker();
	Marker(const QImage& mark);
	Marker(const QImage& mark, const QTransform& transform);

	void place_watermark(QImage& target, const QImage& mark, const QTransform& transform, float opacity = 1.);
	void place_watermark(QImage& target, const QImage& mark, float opacity = 1.);
	void place_watermark(QImage& target, const QTransform& transform, float opacity = 1.);
	void place_watermark(QImage& target, float opacity = 1.);

	inline void set_mark(const QImage& mark){ _mark = mark; }
	inline QImage& get_mark(){ return _mark; }

private:
	QPainter _painter;
	QImage _mark;
	QTransform _tr;
};


inline void Marker::place_watermark(QImage& target, const QImage& mark, float opacity){
	place_watermark(target, mark, _tr, opacity);
}

inline void Marker::place_watermark(QImage& target, const QTransform& transform, float opacity){
	place_watermark(target, _mark, transform, opacity);
}

inline void Marker::place_watermark(QImage& target, float opacity){
	place_watermark(target, _mark, _tr, opacity);
}

#endif // MARKER_H
