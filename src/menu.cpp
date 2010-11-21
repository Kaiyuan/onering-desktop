#include <QAction>
#include "menu.h"
#include "debugger.h"

MenuItem::MenuItem(const QString &title, QObject *parent)
	: QAction(title, parent)
{
}

Menu::Menu(QWidget *parent)
	: QMenu(parent)
{
}

MenuItem * Menu::addItem(const QString &title)
{
	MenuItem *item = new MenuItem(title, this);
	Debugger::traceObj(item);
	this->addAction(item);
	return item;
}
