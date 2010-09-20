#include <QDebug>
#include <QtNetwork>
#include <QIODevice>
#include <QTimer>
#include <string.h>
#include <onering.h>
#include "appreply.h"
#include "app.h"

AppReply::AppReply(const QUrl &url, QObject *parent)
	: QNetworkReply(parent)
{
	AppReply("GET", url, 0, parent);
}

AppReply::AppReply(const char* method, const QUrl &url, QIODevice *body_device, QObject *parent)
	: QNetworkReply(parent)
{
	this->method = method;
	this->url = url;
	this->body_device = body_device;

	if (body_device) {
		qDebug() << "body_device connect";
		connect(body_device, SIGNAL(readyRead()),
				this, SLOT(readBody()));
	} else {
		QTimer::singleShot(0, this, SLOT(parseResponse()));
	}
}

void AppReply::readBody()
{
	qDebug() << "body_device readyRead";
	this->body += this->body_device->readAll();
	if (this->body_device->atEnd()) {
		parseResponse();
	}
}

void AppReply::parseResponse()
{
	QByteArray appname = url.host().toUtf8();
	QByteArray path = url.path().toUtf8();
	content = call_app(appname.constData(), method.toUtf8().constData(),
			path.constData(), body);
	offset = 0;

	open(ReadOnly | Unbuffered);

	// default attribute
	setAttribute(QNetworkRequest::HttpStatusCodeAttribute, 200);
	setHeader(QNetworkRequest::ContentTypeHeader, QVariant("text/html; charset=UTF-8"));

	// TODO speed optimization needed
	int index;
	bool ok;
	int code;
	QByteArray line, key, value;
	QList<QByteArray> words;
	if ((index = content.indexOf("\r\n")) < 0) {
		goto finish;
	}
	line = content.left(index);
	words = line.split(' ');
	if (words.size() < 3) {
		goto finish;
	}
	code = words[1].toInt(&ok);
	if (!ok) {
		goto finish;
	}
//        qDebug() << "code:" << code;
	setAttribute(QNetworkRequest::HttpStatusCodeAttribute, code);
	offset = index + 2;

	// iterate over lines of header
	for(; (index = content.indexOf("\r\n", offset)) > 0; offset=index+2) {
		line = content.mid(offset, index-offset);
		int pos = line.indexOf(':');
		if (pos <= 0) {
			continue;
		}
		key = line.left(pos);
		value = line.mid(pos+1);
//                qDebug() << key << value;
		setRawHeader(key, value);
	}

	if (index == 0) {
		// the CRLF seperate headers and body
		offset += 2;
	}

finish:
//        qDebug() << "body:" << content.mid(offset);
	emit readyRead();
	emit finished();
}

void AppReply::abort()
{
}

qint64 AppReply::bytesAvailable() const
{
	return content.size() - offset;
}

bool AppReply::isSequential() const
{
	return true;
}

qint64 AppReply::readData(char *data, qint64 maxSize)
{
	if (offset < content.size()) {
		qint64 number = qMin(maxSize, content.size() - offset);
		memcpy(data, content.constData() + offset, number);
		offset += number;
		return number;
	} else
		return -1;
}

