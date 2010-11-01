#include <QDebug>
#include <QNetworkRequest>
#include "networkaccessmanager.h"
#include "dataloader.h"

DataLoader::DataLoader(QObject *parent)
	: QObject(parent)
{
}

void DataLoader::load(const QUrl &url)
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
