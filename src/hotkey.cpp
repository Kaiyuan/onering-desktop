#include "hotkey.h"

HotKey::HotKey(const QKeySequence &shortcut, QObject *parent)
	: QxtGlobalShortcut(shortcut, parent)
{
}
