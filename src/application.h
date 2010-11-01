#ifndef APPLICATION_H
#define APPLICATION_H

#include <QApplication>
#include <QObject>

class Application : public QObject
{
	Q_OBJECT

public:
	Application();
	int load(const char* appname);

public slots:
	void setWindowIconByData(QByteArray &data);

};

#endif
