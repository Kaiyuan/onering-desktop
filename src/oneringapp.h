#ifndef ONERINGAPP_H
#define ONERINGAPP_H

#include <onering.h>
#include "app.h"

int onering_app_init(void);

class OneRingApp : public App
{
public:
	OneRingApp(QObject* parent=0);

private:
       	QByteArray processCall(const QString& command, const QVariantMap& param);
	QByteArray _js;
};

void register_onering_app(const char* appname);

#endif
