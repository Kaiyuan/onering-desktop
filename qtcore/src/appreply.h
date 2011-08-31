#ifndef APPREPLY_H
#define APPREPLY_H

#include <QNetworkReply>
#include <QIODevice>

class AppReply : public QNetworkReply
{
	Q_OBJECT

public:
	AppReply(const QUrl &url, QObject *parent=0);
	AppReply(const char* method, const QUrl &url, QIODevice *body_device=0, QObject *parent=0);
	void abort();
	qint64 bytesAvailable() const;
	bool isSequential() const;

protected:
	qint64 readData(char *data, qint64 maxSize);

private slots:
	void readBody();
	void parseResponse();

private:
	QIODevice *body_device;
	QByteArray body;
	QString method;
	QUrl url;
	QByteArray content;
	qint64 offset;
};

#endif
