// includes {{{
#include <QDebug>
#include <QApplication>
#include <qwebview.h>
#include <qwebframe.h>
#include <QWebInspector>
#include <QIcon>
#include <QUrl>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QSystemTrayIcon>
#include <QApplication>
#include "jsapi.h"
#include "oneringview.h"
#include "app.h"
#include "networkaccessmanager.h"
#include "systemtrayicon.h"
#include "menu.h"
#include "hotkey.h"
#include "debugger.h"
#include "application.h"
// }}}

// public methods {{{

JsApi::JsApi(QObject *parent)
	: QObject(parent),
	  inspector(0)
{
}

JsApi::~JsApi()
{
	if (inspector) {
		delete inspector;
	}
}

void JsApi::setWebView(QWebView *view)
{
	QWebPage *page = view->page();
	frame = page->mainFrame();

	attachObject();
	connect(frame, SIGNAL(javaScriptWindowObjectCleared()), this, SLOT(attachObject()));
}

void JsApi::setWindow(QWidget *window)
{
	this->window = window;
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

void JsApi::invokeCallback(const QString &funcname, const QString &param)
{
	qDebug() << "invokeCallback" << funcname << param;

	QString jsparam = param;
	// escape javascript string
	jsparam.replace('\\', "\\\\").replace('\'', "\\'");
	frame->evaluateJavaScript(
			QString("ONERING.callback('%1', '%2'); null;")
			.arg(funcname, jsparam));
}


QObject* JsApi::createWindow(const QString &url, int width, int height, const QVariantMap &props)
{
	qDebug() << "JsApi::createWindow" << url << width << height << props;
	OneRingView *window = new OneRingView(frame->baseUrl().resolved(url),
			width, height, props);
	Debugger::traceObj(window);
	window->show();
	return window;
}

// }}}

// javascript api {{{

// module level {{{

QVariant JsApi::test(QVariant param)
{
	qDebug() << param;
	return QVariant();
}

void JsApi::log(const QString &s)
{
	qDebug() << "JsApi::log" << s;
}

void JsApi::showInspector()
{
	qDebug() << "JsApi::showInspector";
	if (!inspector) {
		inspector = new QWebInspector();
	}
	if (!inspector->page()) {
		inspector->setPage(frame->page());
	}
	inspector->resize(800, 600);
	inspector->show();
	inspector->activateWindow();  // put inspector at the top most
}

void JsApi::exit()
{
	qApp->quit();
}

void JsApi::ajax(const QString &type, const QString &url, const QString &body, const QString &callback)
{
	qDebug() << "JsApi::ajax" << type << url << body << callback;
	QUrl absurl = frame->baseUrl().resolved(url);
	QByteArray response = call_app_body(qPrintable(type), absurl, qPrintable(body));
	invokeCallback(callback, response);
}

bool JsApi::checkAlive(QObject* o)
{
	qDebug() << "JsApi::checkAlive" << o;
	return o != 0;
}

QObject* JsApi::getCurrentWindow()
{
	return window;
}

QObject* JsApi::getApplication()
{
	return qApp;
}

QObject* JsApi::getPubSubHub()
{
	return &(static_cast<Application *>(qApp)->pubsubhub);
}

QString JsApi::resolve(const QString &relative)
{
	return frame->baseUrl().resolved(relative).toString();
}

// }}}

QObject* JsApi::SystemTrayIcon_new()
{
	qDebug() << "JsApi::systemTrayIcon_new";

	SystemTrayIcon *icon = new SystemTrayIcon(qApp);
	Debugger::traceObj(icon);
	return icon;
}

QObject* JsApi::Menu_new()
{
	Menu *menu = new Menu();
	Debugger::traceObj(menu);
	return menu;
}

QObject* JsApi::HotKey_new(const QString &keyseq)
{
	qDebug() << "JsApi::HotKey_new" << keyseq;
	HotKey *hotkey = new HotKey(QKeySequence(keyseq));
	Debugger::traceObj(hotkey);
	return hotkey;
}

// }}}

// vim:set foldmethod=marker:
