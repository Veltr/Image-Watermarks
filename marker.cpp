#include "marker.h"

Marker::Marker(){}

Marker::Marker(const QImage& mark) : _mark(mark) {}

Marker::Marker(const QImage& mark, const QTransform& transform) : _mark(mark){
	_tr = transform;
}

void Marker::place_watermark(QImage& target, const QImage& mark, const QTransform& transform, float opacity){
	if(target.isNull() || mark.isNull()) return;
	_painter.begin(&target);
	_painter.setTransform(transform);
	_painter.setOpacity(opacity);
	_painter.drawImage(0, 0, mark);
	_painter.end();
}


