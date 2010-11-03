#include <QDebug>
#include "debugger.h"

static Debugger *debugger = 0;

void Debugger::traceObj(QObject *obj)
{
#ifdef DEBUG
	qDebug() << "=== NEW" << obj;
	if (!debugger) {
		debugger = new Debugger();
	}
	connect(obj, SIGNAL(destroyed(QObject*)),
			debugger, SLOT(printDestroyed(QObject*)));
#endif
}

void Debugger::printDestroyed(QObject *obj)
{
	qDebug() << "=== DEL" << obj;
}
