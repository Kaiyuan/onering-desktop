#ifndef APPLICATION_H
#define APPLICATION_H

#include <QApplication>
#include <QObject>
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
};

#endif
