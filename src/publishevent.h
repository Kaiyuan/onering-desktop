#ifndef PUBLISHEVENT_H
#define PUBLISHEVENT_H

#include <QEvent>
#include <QString>

class PublishEvent : public QEvent
{
public:
	PublishEvent(const char *channel, const char *msg);

	const QString& channel(void);
	const QString& message(void);
	static int type(void);

private:
	QString _channel;
	QString _msg;
	static int _type;
};

#endif
