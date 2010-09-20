#include <QDebug>
#include <qwebview.h>
#include <qwebframe.h>
#ifdef DEBUG
#include <QWebInspector>
#endif
#include <QIcon>
#include <QUrl>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QSystemTrayIcon>
#include "jsapi.h"
#include "oneringview.h"
#include "networkaccessmanager.h"
#include "systemtrayicon.h"

JsApi::JsApi(QObject *parent)
	: QObject(parent)
{
}

void JsApi::setWebView(QWebView *view)
{
	QWebPage *page = view->page();
	frame = page->mainFrame();

	attachObject();
	connect(frame, SIGNAL(javaScriptWindowObjectCleared()), this, SLOT(attachObject()));
}

void JsApi::attachObject()
{
	frame->addToJavaScriptWindowObject(QString("_OneRing"), this);
}

void JsApi::invokeCallback(const QString &funcname)
{
	qDebug() << "invokeCallback" << funcname;

	frame->evaluateJavaScript(QString("ONERING.callback('%1'); null;").arg(funcname));
}

void JsApi::createWindow(const QString &url, int width, int height, const QString &props_json)
{
	qDebug() << "JsApi::createWindow" << url << width << height << props_json;

	Qt::WindowFlags flags = 0;
	QScriptValue props = parseJSON(props_json);
	if (props.isObject() && props.property("popup").toBool()) {
		flags |= Qt::Popup;
	}
	OneRingView *window = new OneRingView(0, flags);
	window->load(frame->baseUrl().resolved(url));
	window->resize(width, height);
	window->show();
}

void JsApi::log(const QString &s)
{
	qDebug() << "JsApi::log" << s;
}

void JsApi::showInspector()
{
	qDebug() << "JsApi::showInspector";
#ifdef DEBUG
	if (!inspector.page()) {
		inspector.setPage(frame->page());
	}
	inspector.show();
	inspector.activateWindow();  // put inspector at the top most
#endif
}

int JsApi::systemTrayIcon_new()
{
	qDebug() << "JsApi::systemTrayIcon_new";

	SystemTrayIcon *icon = new SystemTrayIcon();
	return (int)icon;
}

void JsApi::systemTrayIcon_load(int handler, const QString &url)
{
	qDebug() << "JsApi::systemTrayIcon_load" << handler << url;

	SystemTrayIcon *icon = (SystemTrayIcon *)handler;
	icon->load(url);
}

void JsApi::systemTrayIcon_bind(int handler, const QString &event, const QString &callback_funcname)
{
	qDebug() << "JsApi::systemTrayIcon_bind" << handler << event << callback_funcname;

	SystemTrayIcon *icon = (SystemTrayIcon *)handler;

	if (event == "click") {
		registerCallback(icon, event, callback_funcname);
		connect(icon, SIGNAL(activated(const QString&)),
			this, SLOT(callback(const QString&)));
	}
}

QString JsApi::systemTrayIcon_getGeometry(int handler)
{
	SystemTrayIcon *icon = (SystemTrayIcon *)handler;

	QRect rect = icon->geometry();
	return QString("{top:%1, left:%2, width:%3, height:%4}")
		.arg(rect.top()).arg(rect.left())
		.arg(rect.width()).arg(rect.height());
}

void JsApi::callback(const QString &event)
{
	qDebug() << "callback" << event;

	QObject *sender = QObject::sender();
	const QList<QString>& funcnames = callbacks[EventSource(sender, event)];
	for (int i = 0; i < funcnames.size(); ++i) {
		invokeCallback(funcnames.at(i));
	}
}

void JsApi::registerCallback(QObject *sender, const QString &event, const QString &callback_funcname)
{
	qDebug() << "registerCallback" << event << callback_funcname;

	callbacks[EventSource(sender, event)] << callback_funcname;
}

QScriptValue JsApi::parseJSON(const QString &json)
{
	return engine.evaluate("("+json+")");
}
