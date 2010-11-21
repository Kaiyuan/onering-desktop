#include "event.h"

Event::Event(QEvent *event, const QString &type, QObject *parent)
	: QObject(parent),
	  event(event),
	  _type(type)
{
}

QString Event::type()
{
	return _type;
}

void Event::preventDefault()
{
	event->ignore();
}
