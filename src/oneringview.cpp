#include <QDebug>
#include <QPair>
#include <QEvent>
#include <QCloseEvent>
#include "oneringview.h"
#include "networkaccessmanager.h"
#include "jsapi.h"
#include "debugger.h"
#include "event.h"

OneRingView::OneRingView(const QUrl &url, int width, int height, const QVariantMap &props)
	: QWebView(), contextMenuEnabled(false)
{
	QNetworkAccessManager *oldManager = page()->networkAccessManager();
	NetworkAccessManager *newManager = new NetworkAccessManager(this, oldManager);
	Debugger::traceObj(newManager);
	page()->setNetworkAccessManager(newManager);

#ifdef DEBUG
	connect(this, SIGNAL(urlChanged(const QUrl &)),
		this, SLOT(printCurrentUrl(const QUrl &)));
#endif

	connect(this, SIGNAL(titleChanged(const QString &)),
		this, SLOT(setWindowTitle(const QString &)));

	jsapi = new JsApi(this);
	Debugger::traceObj(jsapi);
	jsapi->setWebView(this);
	jsapi->setWindow(this);

	setAttribute(Qt::WA_DeleteOnClose, true);

	load(url);
	resize(width, height);

	if (props["fixedSize"].toBool()) {
		setFixedSize(width, height);
	}
	if (!props["title"].toString().isEmpty()) {
		setWindowTitle(props["title"].toString());
	}

	Qt::WindowFlags flags = Qt::Widget;
	if (props.contains("minimizeButton") ||
		       	props.contains("maximizeButton")) {
		flags |= Qt::CustomizeWindowHint
		       	| Qt::WindowTitleHint
		       	| Qt::WindowSystemMenuHint
		       	| Qt::WindowMinimizeButtonHint
		       	| Qt::WindowMaximizeButtonHint
		       	| Qt::WindowCloseButtonHint;
		if (!(props.value("minimizeButton", QVariant(true)).toBool()))
			flags ^= Qt::WindowMinimizeButtonHint;
		if (!(props.value("maximizeButton", QVariant(true)).toBool()))
			flags ^= Qt::WindowMaximizeButtonHint;
	}
	setWindowFlags(flags);

	initializEventMap();
}

void OneRingView::initializEventMap()
{
	eventMap["windowStateChange"] = QEvent::WindowStateChange;
	eventMap["close"] = QEvent::Close;
}

void OneRingView::printCurrentUrl(const QUrl &url)
{
	qDebug() << "nav to:" << url.toString();
}

void OneRingView::contextMenuEvent(QContextMenuEvent *ev)
{
	if (contextMenuEnabled) {
		QWebView::contextMenuEvent(ev);
	}
}

void OneRingView::enableContextMenu()
{
	contextMenuEnabled = true;
}

void OneRingView::disableContextMenu()
{
	contextMenuEnabled = false;
}

void OneRingView::bind(const QString &eventTypeName)
{
	if (eventMap.contains(eventTypeName)) {
		QEvent::Type type = eventMap.value(eventTypeName);
		if (!boundEvents.contains(type)) {
			boundEvents[type].first = eventTypeName;
		}
		boundEvents[type].second += 1;
	}
}

void OneRingView::unbind(const QString &eventTypeName, int times)
{
	if (eventMap.contains(eventTypeName)) {
		QEvent::Type type = eventMap.value(eventTypeName);
		if (boundEvents.contains(type)) {
			boundEvents[type].second -= times;
			if (boundEvents.value(type).second <= 0) {
				boundEvents.remove(type);
			}
		}
	}
}

void OneRingView::changeEvent(QEvent * event)
{
	QEvent::Type type = event->type();
	if (boundEvents.contains(type)) {
		Event e(event, boundEvents.value(type).first);
		emit eventOccurred(&e);
	}
	QWebView::changeEvent(event);
}

void OneRingView::closeEvent(QCloseEvent *event)
{
	QEvent::Type type(event->type());
	if (boundEvents.contains(type)) {
		Event e(reinterpret_cast<QEvent *>(event),
		      boundEvents.value(type).first);
		emit eventOccurred(&e);
	}
}

void OneRingView::activateWindow()
{
	QWebView::activateWindow();
}
