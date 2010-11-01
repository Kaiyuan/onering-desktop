#include <QDebug>
#include <QNetworkRequest>
#include "networkaccessmanager.h"
#include "dataloader.h"

DataLoader::DataLoader(QUrl &url, QObject *parent)
	: QObject(parent)
{
	NetworkAccessManager *manager = new NetworkAccessManager(this);
	connect(manager, SIGNAL(finished(QNetworkReply*)),
			this, SLOT(fetched(QNetworkReply*)));
	manager->get(QNetworkRequest(url));
}

void DataLoader::fetched(QNetworkReply *reply)
{
	QByteArray data = reply->readAll();
	emit got(data);

	reply->manager()->deleteLater();
}
