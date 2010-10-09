#ifndef HOTKEY_H
#define HOTKEY_H

#include <QObject>
#include <QKeySequence>
#include "qxtglobalshortcut.h"

class HotKey : public QxtGlobalShortcut
{
public:
	HotKey(const QKeySequence & shortcut, QObject *parent);
};

#endif
