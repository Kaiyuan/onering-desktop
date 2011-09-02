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
		QString path = dir.filePath(entries[i]);
		if (loadPlugin(path)) {
			n_loaded_plugins += 1;
		} else {
			qDebug() << "load plugin" << path << "failed";
		}
	}

	return n_loaded_plugins;
}

bool PluginLoader::loadPlugin(const QString& path)
{
	QLibrary lib(path);
	return lib.load();
}
