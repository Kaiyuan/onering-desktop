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
class OneRingView;

typedef QPair<QObject*, QString> EventSource;

class JsApi : public QObject
{
	Q_OBJECT

public:
	JsApi(QObject *parent);

	void setWebView(QWebView *view);
	void setWindow(QWidget *window);
	void invokeCallback(const QString &funcname);

public slots:
	void createWindow(const QString &url, int width, int height, const QString &props);
	void Window_hide();
	void Window_maximize();
	void Window_showNormal();

	void log(const QString &s);
	void showInspector(void);
	void exit(void);

	// SystemTrayIcon
	int SystemTrayIcon_new(void);
	void SystemTrayIcon_load(int handler, const QString &url);
	void SystemTrayIcon_bind(int handler, const QString &event, const QString &callback);
	QString SystemTrayIcon_getGeometry(int handler);
	void SystemTrayIcon_setContextMenu(int handler, int menu_handler);

	// Menu
	int Menu_new(void);
	void Menu_addSeparator(int handler);
	void Menu_addItem(int handler, const QString &title, const QString &callback);

private slots:
	void attachObject();
	void callback(const QString &event);
	void callback();

private:
	QWidget *window;
	QWebFrame *frame;
	QWebInspector inspector;
	QHash< EventSource, QList<QString> > callbacks;
	QScriptEngine engine;

	void registerCallback(QObject *sender, const QString &event, const QString &callback_funcname);
	QScriptValue parseJSON(const QString &json);
};

#endif
