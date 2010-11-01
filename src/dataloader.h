#ifndef DATALOADER_H 
#define DATALOADER_H

#include <QObject>
#include <QUrl>
#include <QNetworkReply>

class DataLoader : public QObject
{
	Q_OBJECT

public:
	DataLoader(QUrl &url, QObject *parent=0);

signals:
	void got(QByteArray &data);

private slots:
	void fetched(QNetworkReply *reply);
};


#endif
