#ifndef HOTKEY_H
#define HOTKEY_H

#include <QxtGlobalShortcut>

class HotKey : public QxtGlobalShortcut
{
	public:
		HotKey(const QKeySequence & shortcut, QObject *parent);
};

#endif
