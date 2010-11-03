#ifndef DEBUG_H
#define DEBUG_H

#include <QObject>

class Debugger : public QObject
{
	Q_OBJECT

public:
	static void traceObj(QObject *obj);

public slots:
	void printDestroyed(QObject *obj);
};

#endif
