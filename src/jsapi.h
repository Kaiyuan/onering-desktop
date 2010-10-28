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
	QObject* createWindow(const QString &url, int width, int height, const QString &props);
	void Window_hide();
	void Window_show();
	void Window_maximize();
	void Window_showNormal();
	void Window_enableContextMenu();
	void Window_disableContextMenu();

	void log(const QString &s);
	void showInspector(void);
	void exit(void);
	void post(const QString &url, const QString &body, const QString &callback);
	bool checkAlive(QObject* o=0);
	QObject* getCurrentWindow();

	// SystemTrayIcon
	long SystemTrayIcon_new(void);
	void SystemTrayIcon_delete(long handler);
	void SystemTrayIcon_load(long handler, const QString &url);
	void SystemTrayIcon_bind(long handler, const QString &event, const QString &callback);
	QString SystemTrayIcon_getGeometry(long handler);
	void SystemTrayIcon_setContextMenu(long handler, long menu_handler);

	// Menu
	long Menu_new(void);
	void Menu_delete(long handler);
	void Menu_addSeparator(long handler);
	void Menu_addItem(long handler, const QString &title, const QString &callback, const QVariant &props);
	QObject* Menu_get(long handler, QVariant index);

	long HotKey_new(const QString &keyseq, const QString &callback);
	void HotKey_delete(long handler);

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
