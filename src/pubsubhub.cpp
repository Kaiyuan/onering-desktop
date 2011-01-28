#include <QDebug>
#include "publishevent.h"
#include "pubsubhub.h"

bool PubSubHub::event(QEvent *e)
{
	if (e->type() == PublishEvent::type()) {
		PublishEvent *pe = static_cast<PublishEvent *>(e);
		qDebug() << "emit published" << pe->channel();
		emit published(pe->channel(), pe->message());
	}
	return true;
}
