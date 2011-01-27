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

	Q_INVOKABLE QObject* getPubSubHub(void);
	Q_INVOKABLE QString resolve(const QString &relative);
	Q_INVOKABLE QString call(const QString& method, const QString& url,
			const QString& body) const;

public slots:
	QVariant test(QVariant param);
	QObject* createWindow(const QString &url, int width, int height, const QVariantMap &props);

	void log(const QString &s);
	void showInspector(void);
	void exit(void);
	void ajax(const QString &type, const QString &url, const QString &body, const QString &callback, bool async);
	bool checkAlive(QObject* o=0);
	QObject* getCurrentWindow();
	QObject* getApplication();

	QObject* SystemTrayIcon_new(void);
	QObject* Menu_new(void);
	QObject* HotKey_new(const QString &keyseq);

private slots:
	void attachObject();

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
