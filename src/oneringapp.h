#ifndef ONERINGAPP_H
#define ONERINGAPP_H

#include <onering.h>
#include <QVariantMap>
#include "oneringview.h"
#include "app.h"

int onering_app_init(void);

class OneRingApp : public App
{
public:
	OneRingApp(QObject* parent=0);

	// for application to initilize the first window.
	static void registerWindow(OneRingView *window);

public:
       	QByteArray processCall(const QString& command, const QVariantMap& param);
	QByteArray _js;
};

void register_onering_app(const char* appname);

#endif
