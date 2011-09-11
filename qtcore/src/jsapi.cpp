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
#include "debugger.h"
#include "application.h"
// }}}

// public methods {{{

JsApi::JsApi(QObject *parent)
	: QObject(parent)
{
}

JsApi::~JsApi()
{
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

// }}}

// javascript api {{{

QVariant JsApi::test(QVariant param)
{
	qDebug() << param;
	return QVariant();
}

void JsApi::log(const QString &s)
{
	qDebug() << "JsApi::log" << s;
}

QString JsApi::call(const QString &method, const QString &url, const QString &body) const
{
	qDebug() << "JsApi::call" << method << url << body;
	QUrl absurl = frame->baseUrl().resolved(url);
	QString response = call_app_body(method, absurl, body);
	if (response.isEmpty()) {
		response = "null";
	}
	qDebug() << "JsApi::call" << url << "returns" << response;
	return response;
}

void JsApi::ajax(const QString &type, const QString &url, const QString &body, const QString &callback, bool async)
{
	Q_UNUSED(async);

	qDebug() << "JsApi::ajax" << type << url << body << callback;
	QUrl absurl = frame->baseUrl().resolved(url);
	QByteArray response = call_app_body(type, absurl, body);
	invokeCallback(callback, response);
}

QString JsApi::getCurrentWindowId()
{
	return App::generateObjectId(window);
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

// vim:set foldmethod=marker:
