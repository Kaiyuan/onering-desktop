#ifndef PUBSUBHUB_H
#define PUBSUBHUB_H

#include <QObject>
#include <QEvent>

class PubSubHub : public QObject
{
Q_OBJECT

public:
	bool event(QEvent *e);

signals:
	void published(const QString &channel, const QString &message);
};

#endif
