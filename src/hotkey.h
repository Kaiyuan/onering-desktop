#ifndef HOTKEY_H
#define HOTKEY_H

#include <QObject>
#include <QKeySequence>

class HotKey : public QObject
{
Q_OBJECT

public:
	HotKey(const QKeySequence & shortcut, QObject *parent);

	void setDisabled(void) {};

signals:
	void activated(void);
};

#endif
