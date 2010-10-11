#ifndef ONERINGVIEW_H
#define ONERINGVIEW_H

#include <QWebView>
#include "jsapi.h"

class OneRingView : public QWebView
{
	Q_OBJECT

public:
	OneRingView(const QUrl &url, int width, int height, QVariantMap &props);
	void enableContextMenu();
	void disableContextMenu();

private slots:
	void printCurrentUrl(const QUrl &url);

protected:
	void contextMenuEvent(QContextMenuEvent *ev);

private:
	bool contextMenuEnabled;
};

#endif
