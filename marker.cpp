#include "marker.h"

Marker::Marker(){}

Marker::Marker(const QPixmap& mark) : _mark(mark) {}

Marker::Marker(const QPixmap& mark, const QTransform& transform) : _mark(mark){
	_painter.setTransform(transform);
}

void Marker::place_watermark(QPixmap& target, const QPixmap& mark){
	_painter.begin(&target);
	_painter.drawPixmap(0, 0, mark);
	_painter.end();
}

void Marker::place_watermark(QPixmap& target){
	place_watermark(target, _mark);
}

void Marker::place_watermark(QPixmap& target, const QTransform& transform){
	_painter.setTransform(transform);
	place_watermark(target);
}

void Marker::place_watermark(QPixmap& target, const QPixmap& mark, const QTransform& transform){
	_painter.setTransform(transform);
	place_watermark(target, mark);
}
