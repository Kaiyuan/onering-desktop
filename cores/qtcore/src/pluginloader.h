#ifndef PLUGINLOADER_H
#define PLUGINLOADER_H

#include <QObject>
#include <QDir>
#include <QString>

class PluginLoader : public QObject
{
public:
	static int loadDir(const QDir& dir);
	static bool loadPlugin(const QString& path);
	static bool versionMatch(const QString& pluginRequiredVersion, const QString& coreVersion);

private:
	static int versionCompare(const QString& v1, const QString& v2);
};

#endif
