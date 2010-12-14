#ifndef JSAPI_H
#define JSAPI_H

#include <qobject.h>
#include <QWebInspector>
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
	~JsApi();

	void setWebView(QWebView *view);
	void setWindow(QWidget *window);
	void invokeCallback(const QString &funcname);
	void invokeCallback(const QString &funcname, const QString &param);
	QVariant call(const QString &funcname);
	Q_INVOKABLE QObject* getPubSubHub(void);
	Q_INVOKABLE QString resolve(const QString &relative);

public slots:
	QVariant test(QVariant param);
	QObject* createWindow(const QString &url, int width, int height, const QString &props);
	void Window_enableContextMenu();
	void Window_disableContextMenu();

	void log(const QString &s);
	void showInspector(void);
	void exit(void);
	void ajax(const QString &type, const QString &url, const QString &body, const QString &callback);
	bool checkAlive(QObject* o=0);
	QObject* getCurrentWindow();
	QObject* getApplication();

	// SystemTrayIcon
	QObject* SystemTrayIcon_new(void);

	// Menu
	QObject* Menu_new(void);

	long HotKey_new(const QString &keyseq, const QString &callback);
	void HotKey_delete(long handler);

private slots:
	void attachObject();
	void callback(const QString &event);
	void callback();

private:
	QWidget *window;
	QWebFrame *frame;
	QWebInspector *inspector;
	QHash< EventSource, QList<QString> > callbacks;
	QScriptEngine engine;

	void registerCallback(QObject *sender, const QString &event, const QString &callback_funcname);
	QScriptValue parseJSON(const QString &json);
};

#endif
