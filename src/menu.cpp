#include <QDebug>
#include <QObject>
#include <QString>
#include <QMenu>
#include <QUrl>
#include <QKeySequence>
#include <assert.h>
#include "menu.h"

static MenuApp* g_manager = 0;

MenuApp::MenuApp(QObject* parent)
	: App(parent)
{
}

QByteArray MenuApp::processCall(const QString& command, const QVariantMap& param)
{
	if (command == "Menu.create") {
		return createMenu();
	} else if (command.startsWith("Menu.")) {
		QString id = param.value("id").toString();
		QMenu* menu = static_cast<QMenu *>(getInstance(id));
		if (!menu) {
			return "{\"err\":\"invalid id\"}";
		}
		if (command == "Menu.destroy") {
			return destroyMenu(menu);
		} else if (command == "Menu.addSeparator") {
			return addSeparator(menu);
		} else if (command == "Menu.addMenuItem") {
			return addMenuItem(menu, param.value("text").toString());
		} else if (command == "Menu.getMenuItem") {
			return getMenuItem(menu, param.value("index").toInt());
		}
	} else if (command.startsWith("MenuItem.")) {
		QString id = param.value("id").toString();
		QAction* item = static_cast<QAction *>(getInstance(id));
		if (!item) {
			return "{\"err\":\"invalid id\"}";
		}
		if (command == "MenuItem.setProperties") {
			return setMenuItemProperties(item, param);
		} else if (command == "MenuItem.setText") {
			return setMenuItemText(item, param.value("text").toString());
		}
	}

	return "{\"err\":\"invalid command\"}";
}

QByteArray MenuApp::createMenu()
{
	QMenu* menu = new QMenu();
	return QString("{\"type\":\"Menu\",\"id\":\"%1\"}").arg(getId(menu)).toLatin1();
}

QByteArray MenuApp::destroyMenu(QMenu* menu)
{
	delete menu;
	return "null";
}

QByteArray MenuApp::addSeparator(QMenu* menu)
{
	menu->addSeparator();
	return "null";
}

QByteArray MenuApp::addMenuItem(QMenu* menu, const QString& text)
{
	QAction* action = menu->addAction(text);
	connect(action, SIGNAL(triggered(bool)),
			this, SLOT(menuItemTriggered(bool)));
	return QString("{\"type\":\"MenuItem\",\"id\":\"%1\"}").arg(getId(action)).toLatin1();
}

QByteArray MenuApp::getMenuItem(QMenu* menu, int index)
{
	QList<QAction *> actions = menu->actions();
	if (index < 0 || index >= actions.size()) {
		return "null";
	}
	QAction* action = actions[index];
	return QString("{\"type\":\"MenuItem\",\"id\":\"%1\"}").arg(getId(action)).toLatin1();
}

QByteArray MenuApp::setMenuItemProperties(QAction* item, const QVariantMap& props)
{
	if (props.contains("shortcut")) {
		QString shortcut = props.value("shortcut").toString();
		item->setShortcut(QKeySequence(shortcut));
		item->setShortcutContext(Qt::ApplicationShortcut);
	}
	if (props.contains("enabled")) {
		item->setEnabled(props.value("enabled").toBool());
	}
	if (props.contains("disabled")) {
		item->setEnabled(!(props.value("disabled").toBool()));
	}
	return "null";
}

QByteArray MenuApp::setMenuItemText(QAction* item, const QString& text)
{
	item->setText(text);
	return "null";
}

void MenuApp::menuItemTriggered(bool checked)
{
	QAction* action = static_cast<QAction *>(sender());
	onering_publish(qPrintable(QString("menu.MenuItem.%1.triggered").arg(getId(action))),
		       	qPrintable(QString("%1").arg(checked?"true":"false")));
}

static onering_response_handle_t menu_app(const char* appname, const char* method, 
		const char* path, const char* body,
		const char** response, int* response_len)
{
	if (!g_manager) {
		g_manager = new MenuApp();
	}

	return g_manager->processRequest(appname, method, path, body, response, response_len);
}


static void menu_app_free_response(const char* appname, onering_response_handle_t response_handle)
{
	g_manager->freeResponse(appname, response_handle);
}

void register_menu_app(const char* appname)
{
	onering_register_app(appname, &menu_app, &menu_app_free_response);
}
