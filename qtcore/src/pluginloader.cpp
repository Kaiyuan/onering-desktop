#include <QDir>
#include <QLibrary>
#include <QDebug>
#include <onering.h>
#include "pluginloader.h"

int onering_load_plugins(const char* dir)
{
	int n_loaded_plugins = 0;
	QDir qdir(dir);
	QStringList entries = qdir.entryList();

	for (int i = 0; i < entries.size(); ++i) {
		QString path = qdir.filePath(entries[i]);
		QLibrary lib(path);
		if (lib.load()) {
			n_loaded_plugins += 1;
		} else {
			qDebug() << "load plugin" << path << "failed";
		}
	}

	return n_loaded_plugins;
}

