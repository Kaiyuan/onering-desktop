#ifndef MENU_H
#define MENU_H

#include <onering.h>
#include <QSet>
#include <QMenu>
#include <QScriptEngine>
#include <QVariantMap>

class MenuManager : public QObject
{
Q_OBJECT

public:
	MenuManager(QObject* parent=0);
	~MenuManager();

	onering_response_handle_t processRequest(const char* appname, const char* method, const QString& path, const QByteArray& body, const char** response, int* response_len);
	void freeResponse(const char* appname, onering_response_handle_t response_handle);

private:
	QByteArray createMenu();
	QByteArray destroyMenu(QMenu* menu);
	QByteArray addSeparator(QMenu* menu);
	QByteArray addMenuItem(QMenu* menu, const QString& text);
	QByteArray getMenuItem(QMenu* menu, int index);

	QByteArray setMenuItemProperties(QAction* action, const QVariantMap& props);
	QByteArray setMenuItemText(QAction* action, const QString& text);

private slots:
	void menuItemTriggered(bool checked=false);

private:
	QByteArray processCall(const QString& command, const QVariantMap& param);
	QString getId(QObject* obj);
	QObject* getInstance(const QString& id);
	QScriptEngine engine;

};

void register_menu_app(const char* appname);

#endif
