#include <QDebug>
#include <QtNetwork>
#include <QIODevice>
#include <onering.h>
#include "networkaccessmanager.h"
#include "appreply.h"
#include "app.h"
#include "debugger.h"

NetworkAccessManager::NetworkAccessManager(QObject *parent, QNetworkAccessManager *manager)
	: QNetworkAccessManager(parent)
{
	if (manager) {
		setCache(manager->cache());
		setCookieJar(manager->cookieJar());
		setProxy(manager->proxy());
		setProxyFactory(manager->proxyFactory());
	}

	methods[QNetworkAccessManager::HeadOperation] = "HEAD";
	methods[QNetworkAccessManager::GetOperation] = "GET";
	methods[QNetworkAccessManager::PutOperation] = "PUT";
	methods[QNetworkAccessManager::PostOperation] = "POST";
	methods[QNetworkAccessManager::DeleteOperation] = "DELETE";
}

QNetworkReply *NetworkAccessManager::createRequest(
		QNetworkAccessManager::Operation operation,
		const QNetworkRequest &request,
		QIODevice *device)
{
	const QUrl& url = request.url();
	if (is_appname_registered(url.host())) {
//                qDebug() << "delegate to app" << url.toString();
		AppReply *reply = new AppReply(methods[operation], url, device, this);
		Debugger::traceObj(reply);
		return reply;
	}

	return QNetworkAccessManager::createRequest(operation, request, device);
}
