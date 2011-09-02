#ifndef PLUGINLOADER_H
#define PLUGINLOADER_H

class PluginLoader : public QObject
{
public:
	static int loadDir(const QDir& dir);
	static bool loadPlugin(const QString& path);
};

#endif
