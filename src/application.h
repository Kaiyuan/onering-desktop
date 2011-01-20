#ifndef APPLICATION_H
#define APPLICATION_H

#include <QApplication>
#include <QObject>

#ifdef Q_WS_MAC
#include <Carbon/Carbon.h>
#endif

#include "pubsubhub.h"

class Application : public QApplication
{
	Q_OBJECT

public:
	Application(int &argc, char **argv);
	~Application();

	int load(const char* appname);
	PubSubHub pubsubhub;
	bool quiting;

public slots:
	void setWindowIconByData(QByteArray &data);

#ifdef Q_WS_MAC
public:
	void emitReopen(void);
	bool macEventFilter(EventHandlerCallRef caller, EventRef event);
signals:
	void dockClicked(void);
private:
	AEEventHandlerUPP m_appleEventProcessorUPP;
#endif
};

#endif
