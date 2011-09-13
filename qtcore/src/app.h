#ifndef APP_H
#define APP_H

#include <onering.h>
#include <QString>
#include <QUrl>
#include <QByteArray>
#include <QVariantMap>
#include <QEvent>
#include <QSet>

int is_appname_registered(const QString &appname);
int get_app(const char* appname, onering_app_func_t* app, onering_free_response_func_t* free_respnose);

QByteArray call_app(const QString& method, const QUrl &url, const QString &body);
QByteArray call_app_body(const QString &method, const QUrl &url, const QString &body);

class App : public QObject
{
	Q_OBJECT

public:
	App(const QString& appname, QObject *parent=0);

	void* processRequest(const char* appname, const char* method, const QString& path, const QByteArray& body, const char** response, int* response_len);
	void freeResponse(const char* appname, void* response_handle);

	static QString generateObjectId(void* obj);

private slots:
	void instanceDestroyed(QObject* obj);

protected:
	QString getId(QObject* obj);
	QString getId(void* obj);
	void* getInstance(const QString& id);
	void publishEvent(const QString& type, void* sender, const QString& event);
	void publishEvent(const QString& type, void* sender, const QString& event, QEvent* e);
	void publishEvent(const QString& type, void* sender, const QString& event, const QString& data);

private:
	virtual QByteArray processCall(const QString& command, const QVariantMap& param) = 0;

	QString appname;
};

#endif
