#ifndef ONERINGVIEW_H
#define ONERINGVIEW_H

#include <QWebView>
#include <QHash>
#include <QEvent>
#include <QList>
#include "jsapi.h"

class OneRingView : public QWebView
{
	Q_OBJECT

public:
	OneRingView(const QUrl &url, int width, int height, QVariantMap &props);
	void enableContextMenu();
	void disableContextMenu();

public slots:
	void bind(const QString &eventType, const QString &callback);
	void activateWindow(void);

private slots:
	void printCurrentUrl(const QUrl &url);

protected:
	void contextMenuEvent(QContextMenuEvent *ev);
	void changeEvent(QEvent* event);
	void initializEventMap(void);

private:
	bool contextMenuEnabled;
	QHash<QEvent::Type, QList<QString> > boundEvents;
	QHash<QString, QEvent::Type> eventMap;
	JsApi *jsapi;
};

#endif
