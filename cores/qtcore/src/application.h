#ifndef APPLICATION_H
#define APPLICATION_H

#include <QApplication>
#include <QObject>
#include "pubsubhub.h"
#ifdef Q_WS_MAC
#ifdef QT_MAC_USE_COCOA
#include "dockiconclick.h"
#endif
#endif

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

signals:
#ifdef Q_WS_MAC
#ifdef QT_MAC_USE_COCOA
	void dockIconClicked();
#endif
#endif

#ifdef Q_WS_MAC
#ifdef QT_MAC_USE_COCOA
private:
	DockIconClickMonitor _dockIconClickMonitor;
#endif
#endif
	
};

#endif
