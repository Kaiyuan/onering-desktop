#ifndef MENU_H
#define MENU_H

#include <QMenu>
#include <QAction>

class MenuItem : public QAction
{
	public:
		MenuItem(const QString &title, QObject *parent=0);
};

class Menu : public QMenu
{
public:
	Menu(QWidget *parent=0);

	MenuItem * addItem(const QString &title);
};

#endif
