#ifndef MENU_H
#define MENU_H

#include <onering.h>
#include <QSet>
#include <QMenu>
#include <QScriptEngine>
#include <QVariantMap>
#include "app.h"

class MenuManager : public App
{
Q_OBJECT

public:
	MenuManager(QObject* parent=0);

private:
	QByteArray createMenu();
	QByteArray destroyMenu(QMenu* menu);
	QByteArray addSeparator(QMenu* menu);
	QByteArray addMenuItem(QMenu* menu, const QString& text);
	QByteArray getMenuItem(QMenu* menu, int index);

	QByteArray setMenuItemProperties(QAction* action, const QVariantMap& props);
	QByteArray setMenuItemText(QAction* action, const QString& text);

private slots:
	void menuItemTriggered(bool checked=false);

private:
	QByteArray processCall(const QString& command, const QVariantMap& param);
};

void register_menu_app(const char* appname);

#endif
