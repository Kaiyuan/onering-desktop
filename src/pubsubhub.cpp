#include <QDebug>
#include "publishevent.h"
#include "pubsubhub.h"

void PubSubHub::publish(const QString& channel, const QString& message)
{
	qDebug() << "publish" << channel << message;
	emit published(channel, message);
}
