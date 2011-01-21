#include "menuitem.h"

MenuItem::MenuItem(QAction* action, QObject* parent)
	: QObject(parent),
	  _action(action)
{
}

MenuItem::~MenuItem()
{
}

QObject* MenuItem::action()
{
	return _action;
}

void MenuItem::setShortcut(const QString &shortcut)
{
	_action->setShortcut(QKeySequence(shortcut));
}

QString MenuItem::shortcut() const
{
	return (_action->shortcut()).toString();
}
