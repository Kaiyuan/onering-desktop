// includes {{{
#include <QDebug>
#include <QApplication>
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
#include <QApplication>
#include "jsapi.h"
#include "oneringview.h"
#include "app.h"
#include "networkaccessmanager.h"
#include "systemtrayicon.h"
#include "menu.h"
#include "hotkey.h"
#include "debugger.h"
// }}}

// public methods {{{

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


QVariant JsApi::call(const QString &funcname)
{
	qDebug() << "call" << funcname;

	return frame->evaluateJavaScript(QString("ONERING.%1()").arg(funcname));
}

QObject* JsApi::createWindow(const QString &url, int width, int height, const QString &props_json)
{
	qDebug() << "JsApi::createWindow" << url << width << height << props_json;
	QVariantMap props = parseJSON(props_json).toVariant().toMap();
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
#ifdef DEBUG
	if (!inspector.page()) {
		inspector.setPage(frame->page());
	}
	inspector.resize(800, 600);
	inspector.show();
	inspector.activateWindow();  // put inspector at the top most
#endif
}

void JsApi::exit()
{
	qApp->quit();
}

void JsApi::ajax(const QString &type, const QString &url, const QString &body, const QString &callback)
{
	qDebug() << "JsApi::ajax" << type << url << body << callback;
	QUrl absurl = frame->baseUrl().resolved(url);
	QByteArray response = call_app_body(qPrintable(absurl.host()),
			qPrintable(type), qPrintable(absurl.path()),
			qPrintable(body));
	invokeCallback(callback, response);
}

bool JsApi::checkAlive(QObject* o)
{
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

// }}}

// Window {{{
void JsApi::Window_enableContextMenu()
{
	static_cast<OneRingView*>(window)->enableContextMenu();
}

void JsApi::Window_disableContextMenu()
{
	static_cast<OneRingView*>(window)->disableContextMenu();
}

// }}}

// SystemTrayIcon {{{
long JsApi::SystemTrayIcon_new()
{
	qDebug() << "JsApi::systemTrayIcon_new";

	SystemTrayIcon *icon = new SystemTrayIcon(qApp);
	Debugger::traceObj(icon);
	return (long)icon;
}

void JsApi::SystemTrayIcon_delete(long handler)
{
	qDebug() << "JsApi::SystemTrayIcon_delete";
	SystemTrayIcon *icon = (SystemTrayIcon *)handler;
	delete icon;
}

void JsApi::SystemTrayIcon_load(long handler, const QString &url)
{
	qDebug() << "JsApi::systemTrayIcon_load" << handler << url;

	SystemTrayIcon *icon = (SystemTrayIcon *)handler;
	icon->load(frame->baseUrl().resolved(url));
}

void JsApi::SystemTrayIcon_bind(long handler, const QString &event, const QString &callback_funcname)
{
	qDebug() << "JsApi::systemTrayIcon_bind" << handler << event << callback_funcname;

	SystemTrayIcon *icon = (SystemTrayIcon *)handler;

	if (event == "click") {
		registerCallback(icon, event, callback_funcname);
		connect(icon, SIGNAL(activated(const QString&)),
			this, SLOT(callback(const QString&)));
	}
}

void JsApi::SystemTrayIcon_setContextMenu(long handler, long menu_handler)
{
	SystemTrayIcon *icon = (SystemTrayIcon *)handler;
	Menu *menu = (Menu *)menu_handler;
	icon->setContextMenu(menu);
}

QString JsApi::SystemTrayIcon_getGeometry(long handler)
{
	SystemTrayIcon *icon = (SystemTrayIcon *)handler;

	QRect rect = icon->geometry();
	return QString("{top:%1, left:%2, width:%3, height:%4}")
		.arg(rect.top()).arg(rect.left())
		.arg(rect.width()).arg(rect.height());
}
//}}}

// Menu {{{
long JsApi::Menu_new()
{
	Menu *menu = new Menu();
	Debugger::traceObj(menu);
	return (long)menu;
}

void JsApi::Menu_delete(long handler)
{
	qDebug() << "JsApi::Menu_delete";
	Menu *menu = (Menu *)handler;
	delete menu;
}

void JsApi::Menu_addSeparator(long handler)
{
	Menu *menu = (Menu *)handler;
	menu->addSeparator();
}

void JsApi::Menu_addItem(long handler, const QString &title, const QString &callback, const QVariant &props)
{
	Menu *menu = (Menu *)handler;
	MenuItem *item = menu->addItem(title);
	QVariantMap vpm = props.toMap();
	QVariant v;
	if ((v = vpm["shortcut"]).isValid()) {
		item->setShortcut(v.toString());
		item->setShortcutContext(Qt::ApplicationShortcut);
	}
	if ((v = vpm["enabled"]).isValid()) {
		item->setEnabled(v.toBool());
	}
	if ((v = vpm["disabled"]).isValid()) {
		item->setDisabled(v.toBool());
	}
	if (!callback.isEmpty()) {
		registerCallback(item, "", callback);
		connect(item, SIGNAL(triggered()), this, SLOT(callback()));
	}
}

QObject* JsApi::Menu_get(long handler, QVariant index)
{
	qDebug() << "JsApi::Menu_get" << handler << index;
	Menu *menu = (Menu *)handler;
	return menu->actions()[index.toInt()];
}

// }}}

// HotKey {{{
long JsApi::HotKey_new(const QString &keyseq, const QString &callback)
{
	qDebug() << "JsApi::HotKey_new" << keyseq << callback;
	HotKey *hotkey = new HotKey(QKeySequence(keyseq), this);
	Debugger::traceObj(hotkey);
	registerCallback(hotkey, "", callback);
	connect(hotkey, SIGNAL(activated()), this, SLOT(callback()));
	return (long)hotkey;
}

void JsApi::HotKey_delete(long handler)
{
	qDebug() << "JsApi::HotKey_delete" << handler;
	HotKey *hotkey = (HotKey *)handler;
	hotkey->setDisabled();
	delete hotkey;
}

//}}}

// }}}

// private methods {{{

void JsApi::callback(const QString &event)
{
	qDebug() << "callback" << event;

	QObject *sender = QObject::sender();
	const QList<QString>& funcnames = callbacks[EventSource(sender, event)];
	for (int i = 0; i < funcnames.size(); ++i) {
		invokeCallback(funcnames.at(i));
	}
}

void JsApi::callback()
{
	callback("");
}

void JsApi::registerCallback(QObject *sender, const QString &event, const QString &callback_funcname)
{
	if (!callback_funcname.isEmpty()) {
		qDebug() << "registerCallback" << sender << event << callback_funcname;
		callbacks[EventSource(sender, event)] << callback_funcname;
	}
}

QScriptValue JsApi::parseJSON(const QString &json)
{
	return engine.evaluate("("+json+")");
}

// }}}

// vim:set foldmethod=marker:
