#ifndef MENU_H
#define MENU_H

#include <QObject>
#include <QMenu>
#include <QAction>

class Menu : public QMenu
{
	Q_OBJECT

public:
	Menu(QWidget *parent=0);

	Q_INVOKABLE QObject* addSeparator(void);
	Q_INVOKABLE QObject* addAction(const QString &text);
	Q_INVOKABLE QObject* get(int index);
};

#endif
