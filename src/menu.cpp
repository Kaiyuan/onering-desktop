#include <QList>
#include <QAction>
#include "menu.h"
#include "menuitem.h"
#include "debugger.h"

Menu::Menu(QWidget *parent)
	: QMenu(parent)
{
}

QObject* Menu::addSeparator()
{
	return QMenu::addSeparator();
}

QObject* Menu::addAction(const QString &text)
{
	QAction* action = QMenu::addAction(text);
	return new MenuItem(action, action);
}

QObject* Menu::get(int index)
{
	QList<QAction *> actions = this->actions();
	if (index < 0 || index >= actions.size()) {
		return 0;
	} else {
		QAction* action = actions[index];
		return new MenuItem(action, action);
	}
}
