#ifndef ONERINGVIEW_H
#define ONERINGVIEW_H

#include <QWebView>
#include "jsapi.h"

class OneRingView : public QWebView
{
	Q_OBJECT

public:
	OneRingView(QWidget* parent=0, Qt::WindowFlags f=0);

private slots:
	void printCurrentUrl(const QUrl &url);

};

#endif
