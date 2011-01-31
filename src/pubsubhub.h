#ifndef PUBSUBHUB_H
#define PUBSUBHUB_H

#include <QObject>

class PubSubHub : public QObject
{
Q_OBJECT

public:
	void publish(const QString &channel, const QString &message);

signals:
	void published(const QString &channel, const QString &message);
};

#endif
