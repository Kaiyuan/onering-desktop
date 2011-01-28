#include <QObject>
#include <QString>
#include <QMenu>
#include <QUrl>
#include <assert.h>
#include "menu.h"

static MenuManager* g_manager = 0;

MenuManager::MenuManager(QObject* parent)
	: QObject(parent)
{
}

MenuManager::~MenuManager()
{
}

onering_response_handle_t MenuManager::processRequest(const char* appname,
		const char* method, const QString& path, const QByteArray& body,
	       	const char** response, int* response_len)
{
	Q_UNUSED(appname);
	Q_UNUSED(method);

	QByteArray* res = new QByteArray();
	QVariantMap param = engine.evaluate("("+body+")").toVariant().toMap();
	QString id = param.value("id").toString();
	QMenu *menu = 0;
	QAction *item = 0;
	if (!id.isEmpty()) {
		if (path.startsWith("/Menu.")) {
			menu = static_cast<QMenu *>(getInstance(id));
		} else if (path.startsWith("/MenuItem.")) {
			item = static_cast<QAction *>(getInstance(id));
		}
	}

	if (path == "/Menu.create") {
		*res = createMenu();
	} else if (path == "/Menu.destroy") {
		*res = destroyMenu(menu);
	} else if (path == "/Menu.addSeparator") {
		*res = addSeparator(menu);
	} else if (path == "/Menu.addMenuItem") {
		*res = addMenuItem(menu, param.value("text").toString());
	} else if (path == "/Menu.getMenuItem") {
		*res = getMenuItem(menu, param.value("index").toInt());
	} else if (path == "/MenuItem.setProperties") {
		*res = setMenuItemProperties(item, param);
	} else if (path == "/MenuItem.setText") {
		*res = setMenuItemText(item, param.value("text").toString());
	}
	*response = res->constData();
	*response_len = res->size();
	return reinterpret_cast<onering_response_handle_t>(res);
}

void MenuManager::freeResponse(const char* appname, onering_response_handle_t handle)
{
	Q_UNUSED(appname);

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

QByteArray MenuManager::setMenuItemProperties(QAction* item, const QVariantMap& props)
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

QByteArray MenuManager::setMenuItemText(QAction* item, const QString& text)
{
	item->setText(text);
	return "null";
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

QObject* MenuManager::getInstance(const QString& id)
{
	bool ok;
	return reinterpret_cast<QObject *>(id.toInt(&ok, 16));
}

static onering_response_handle_t menu_app(const char* appname, const char* method, 
		const char* path, const char* body,
		const char** response, int* response_len)
{
	if (!g_manager) {
		g_manager = new MenuManager();
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
