#include <QDir>
#include <QLibrary>
#include <QDebug>
#include <onering.h>
#include "pluginloader.h"

int PluginLoader::loadDir(const QDir& dir)
{
	int n_loaded_plugins = 0;
	QStringList entries = dir.entryList();

	for (int i = 0; i < entries.size(); ++i) {
		if (entries[i] == "." || entries[i] == "..")
			continue;
		QString path = dir.filePath(entries[i]);
		if (loadPlugin(path)) {
			n_loaded_plugins += 1;
		}
	}

	return n_loaded_plugins;
}

bool PluginLoader::loadPlugin(const QString& path)
{
	QLibrary lib(path);
	if (!lib.load()) {
		qDebug() << "load plugin" << path << "failed";
		return false;
	}

	require_onering_version_func_t require_onering_version = (require_onering_version_func_t) lib.resolve("require_onering_version");
	get_onering_appname_func_t get_onering_appname = (get_onering_appname_func_t) lib.resolve("get_onering_appname");
	register_onering_plugin_func_t register_onering_plugin = (register_onering_plugin_func_t) lib.resolve("register_onering_plugin");

	if (!require_onering_version || !get_onering_appname || !register_onering_plugin) {
		qDebug() << path << "is not an onering plugin";
		lib.unload();
		return false;
	}

	const QString required_version = require_onering_version();
	if (!versionMatch(required_version, onering_version())) {
		qDebug() << path << "requires" << required_version << ", not me";
		lib.unload();
		return false;
	}

	if (register_onering_plugin(get_onering_appname())) {
		qDebug() << path << "register failed";
		lib.unload();
		return false;
	}

	return true;
}

bool PluginLoader::versionMatch(const QString& pluginRequiredVersion, const QString& coreVersion)
{
	QStringList coreV = coreVersion.split(' ', QString::SkipEmptyParts);
	if (coreV.size() != 2) {
		qDebug() << "Core version" << coreVersion << "is invalid";
		return false;
	}

	QStringList patterns = pluginRequiredVersion.split('|');
	for (int i = 0; i < patterns.size(); ++i) {
		QStringList require = patterns[i].split(' ', QString::SkipEmptyParts);
		if (require.size() == 2 && require[0] == coreV[0] && versionCompare(require[1], coreV[1]) <= 0) {
			return true;
		}
	}
	return false;
}

// compare between versions like "1.0.0" and "0.9".
int PluginLoader::versionCompare(const QString& v1, const QString& v2)
{
	int i1, i2;
	QStringList l1 = v1.split('.');
	QStringList l2 = v2.split('.');

	for (int i = 0; ; ++i) {
		if (i >= l1.size()) {
			return (i >= l2.size()) ? 0 : -1;
		} else if (i >= l2.size()) {
			return 1;
		}
		
		i1 = l1[i].toInt();
		i2 = l2[i].toInt();
		if (i1 > i2) {
			return 1;
		} else if (i1 < i2) {
			return -1;
		}
	}
}
