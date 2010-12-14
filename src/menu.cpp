#include <QList>
#include <QAction>
#include "menu.h"
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
	return QMenu::addAction(text);
}

QObject* Menu::get(int index)
{
	QList<QAction *> actions = this->actions();
	if (index < 0 || index >= actions.size()) {
		return 0;
	} else {
		return actions[index];
	}
}
