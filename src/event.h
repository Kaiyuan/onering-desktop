#ifndef EVENT_H
#define EVENT_H

#include <QObject>
#include <QEvent>

class Event : public QObject
{
	Q_OBJECT

public:
	Event(QEvent *event, const QString &type, QObject *parent=0);

	Q_INVOKABLE QString type(void);
	Q_INVOKABLE void preventDefault(void);

private:
	QEvent *event;
	const QString _type;
};

#endif
