#include <QDebug>
#include <QEvent>
#include "oneringview.h"
#include "networkaccessmanager.h"
#include "jsapi.h"

OneRingView::OneRingView(const QUrl &url, int width, int height, QVariantMap &props)
	: QWebView(), contextMenuEnabled(false)
{
	QNetworkAccessManager *oldManager = page()->networkAccessManager();
	NetworkAccessManager *newManager = new NetworkAccessManager(this, oldManager);
	page()->setNetworkAccessManager(newManager);

#ifdef DEBUG
	connect(this, SIGNAL(urlChanged(const QUrl &)),
		this, SLOT(printCurrentUrl(const QUrl &)));
#endif

	connect(this, SIGNAL(titleChanged(const QString &)),
		this, SLOT(setWindowTitle(const QString &)));

	jsapi = new JsApi(this);
	jsapi->setWebView(this);
	jsapi->setWindow(this);

	this->setAttribute(Qt::WA_DeleteOnClose, true);

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
	if (!eventMap.contains(eventTypeName)) {
		return;
	}

	boundEvents[eventMap[eventTypeName]] = eventTypeName;
}

void OneRingView::changeEvent(QEvent * event)
{
	QEvent::Type type = event->type();
	if (boundEvents.contains(type)) {
		emit eventOccurred(boundEvents[type]);
		bool accepted = jsapi->call("getLastEventAccepted").toBool();
		if (!accepted) {
			qDebug() << "event ignored";
			event->ignore();
		}
	}
	QWebView::changeEvent(event);
}
