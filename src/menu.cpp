#include <string.h>
#include <QObject>
#include <QString>
#include <QMenu>
#include <QUrl>
#include <assert.h>
#include "menu.h"

#define APPNAME "menu"

static MenuManager* g_manager = 0;

MenuManager::MenuManager(QObject* parent)
	: QObject(parent)
{
}

MenuManager::~MenuManager()
{
}

onering_response_handle_t MenuManager::processRequest(const char* appname,
		const char* method, const char* path, const char* body,
	       	const char** response, int* response_len)
{
	Q_UNUSED(method);

	assert(strcmp(appname, APPNAME) == 0);

	QByteArray* res = new QByteArray();
	QUrl url = QUrl::fromEncoded(body);
	QString id = url.queryItemValue("id");
	QMenu *menu = 0;
	if (!id.isEmpty()) {
		menu = getInstance(id);
	}

	if (strcmp(path, "/Menu.create") == 0) {
		*res = createMenu();
	} else if (strcmp(path, "/Menu.destroy") == 0) {
		*res = destroyMenu(menu);
	} else if (strcmp(path, "/Menu.addSeparator") == 0) {
		*res = addSeparator(menu);
	} else if (strcmp(path, "/Menu.addMenuItem") == 0) {
		*res = addMenuItem(menu, url.queryItemValue("text"));
	} else if (strcmp(path, "/Menu.getMenuItem") == 0) {
		*res = getMenuItem(menu, url.queryItemValue("index").toInt());
	}
	*response = res->constData();
	*response_len = res->size();
	return reinterpret_cast<onering_response_handle_t>(res);
}

void MenuManager::freeResponse(const char* appname, onering_response_handle_t handle)
{
	Q_UNUSED(appname);

	assert(strcmp(appname, APPNAME) == 0);

	delete reinterpret_cast<QByteArray *>(handle);
}

QByteArray MenuManager::createMenu()
{
	QMenu* menu = new QMenu();
	return QString("{\"id\":\"%1\"}").arg(getId(menu)).toLatin1();
}

QByteArray MenuManager::destroyMenu(QMenu* menu)
{
	delete menu;
	return "null";
}

QByteArray MenuManager::addSeparator(QMenu* menu)
{
	menu->addSeparator();
	return "null";
}

QByteArray MenuManager::addMenuItem(QMenu* menu, const QString& text)
{
	QAction* action = menu->addAction(text);
	connect(action, SIGNAL(triggered(bool)),
			this, SLOT(menuItemTriggered(bool)));
	return QString("{\"item_id\":\"%1\"}").arg(getId(action)).toLatin1();
}

QByteArray MenuManager::getMenuItem(QMenu* menu, int index)
{
	QList<QAction *> actions = menu->actions();
	if (index < 0 || index >= actions.size()) {
		return "null";
	}
	QAction* action = actions[index];
	return QString("{\"item_id\":\"%1\"}").arg(getId(action)).toLatin1();
}

void MenuManager::menuItemTriggered(bool checked)
{
	QAction* action = static_cast<QAction *>(sender());
	onering_publish(qPrintable(QString("MenuItem.%1.triggered").arg(getId(action))),
		       	qPrintable(QString("%1").arg(checked?"true":"false")));
}

QString MenuManager::getId(QObject* obj)
{
	return QString::number(reinterpret_cast<long>(obj), 16);
}

QMenu* MenuManager::getInstance(const QString& id)
{
	bool ok;
	return reinterpret_cast<QMenu*>(id.toInt(&ok, 16));
}

onering_response_handle_t menu_app(const char* appname, const char* method, 
		const char* path, const char* body,
		const char** response, int* response_len)
{
	if (!g_manager) {
		g_manager = new MenuManager();
	}

	return g_manager->processRequest(appname, method, path, body, response, response_len);
}


void menu_app_free_response(const char* appname, onering_response_handle_t response_handle)
{
	g_manager->freeResponse(appname, response_handle);
}

