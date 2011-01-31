#ifndef ONERINGVIEW_H
#define ONERINGVIEW_H

#include <QWebView>
#include <QHash>
#include <QEvent>
#include <QList>
#include <QPair>
#include <QWebInspector>
#include "jsapi.h"

class OneRingView : public QWebView
{
	Q_OBJECT

public:
	OneRingView(const QVariantMap &props);
	~OneRingView();

	void setProperties(const QVariantMap &props);
	void showInspector(void);

signals:
	void eventOccurred(QEvent *event, const QString& type);

public slots:
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
	JsApi *jsapi;
	QWebInspector *inspector;
};

#endif
