#ifndef ONERINGVIEW_H
#define ONERINGVIEW_H

#include <QWebView>
#include <QHash>
#include <QEvent>
#include <QList>
#include <QPair>
#include "jsapi.h"

class OneRingView : public QWebView
{
	Q_OBJECT

public:
	OneRingView(const QUrl &url, int width, int height, const QVariantMap &props);
	Q_INVOKABLE void enableContextMenu();
	Q_INVOKABLE void disableContextMenu();

signals:
	void eventOccurred(QObject *event);

public slots:
	void bind(const QString &eventTypeName);
	void unbind(const QString &eventTypeName, int times=1);
	void activateWindow(void);

private slots:
	void printCurrentUrl(const QUrl &url);

protected:
	void contextMenuEvent(QContextMenuEvent *ev);
	void changeEvent(QEvent* event);
	void closeEvent(QCloseEvent* event);
	void initializEventMap(void);

private:
	bool contextMenuEnabled;
	QHash<QEvent::Type, QPair<QString, int> > boundEvents;
	QHash<QString, QEvent::Type> eventMap;
	JsApi *jsapi;
};

#endif
