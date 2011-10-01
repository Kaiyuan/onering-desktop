#ifndef NETWORKACCESSMANAGER_H
#define NETWORKACCESSMANAGER_H

#include <QNetworkAccessManager>

class NetworkAccessManager : public QNetworkAccessManager
{
	Q_OBJECT

public:
	NetworkAccessManager(QObject *parent = 0, QNetworkAccessManager *oldManager=0);

protected:
	QNetworkReply *createRequest(Operation operation, const QNetworkRequest &request, QIODevice *device);

private:
	const char* methods[10];
};

#endif
