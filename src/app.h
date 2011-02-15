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

QByteArray call_app(const char* method, const QUrl &url, const char* body=NULL);
QByteArray call_app_body(const char* method, const QUrl &url, const char* body=NULL);

class App : public QObject
{
	Q_OBJECT

public:
	App(const QString& appname, QObject *parent=0);

	onering_response_handle_t processRequest(const char* appname, const char* method, const QString& path, const QByteArray& body, const char** response, int* response_len);
	void freeResponse(const char* appname, onering_response_handle_t response_handle);

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
