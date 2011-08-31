#include <QDebug>
#include "publishevent.h"

int PublishEvent::_type = QEvent::registerEventType();

PublishEvent::PublishEvent(const char *channel, const char *msg)
	: QEvent(static_cast<QEvent::Type>(_type)),
	  _channel(channel),
	  _msg(msg)
{
}

const QString& PublishEvent::channel()
{
	return _channel;
}

const QString& PublishEvent::message()
{
	return _msg;
}

int PublishEvent::type()
{
	return _type;
}
