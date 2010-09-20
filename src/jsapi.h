#ifndef JSAPI_H
#define JSAPI_H

#include <qobject.h>
#ifdef DEBUG
#include <QWebInspector>
#endif
#include <QHash>
#include <QList>
#include <QScriptEngine>

class QWebView;
class QWebFrame;

typedef QPair<QObject*, QString> EventSource;

class JsApi : public QObject
{
	Q_OBJECT

public:
	JsApi(QObject *parent);

	void setWebView(QWebView *view);
	void invokeCallback(const QString &funcname);

public slots:
	void createWindow(const QString &url, int width, int height, const QString &props);
	void log(const QString &s);
	void showInspector(void);
	int systemTrayIcon_new(void);
	void systemTrayIcon_load(int handler, const QString &url);
	void systemTrayIcon_bind(int handler, const QString &event, const QString &callback);
	QString systemTrayIcon_getGeometry(int handler);

private slots:
	void attachObject();
	void callback(const QString &event);

private:
	QWebFrame *frame;
	QWebInspector inspector;
	QHash< EventSource, QList<QString> > callbacks;
	QScriptEngine engine;

	void registerCallback(QObject *sender, const QString &event, const QString &callback_funcname);
	QScriptValue parseJSON(const QString &json);
};

#endif
