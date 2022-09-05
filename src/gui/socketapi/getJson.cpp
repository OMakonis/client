#include "getJson.h"
#include "guiutility.h"

#include <QDesktopServices>
#include <QUrl>


void getJson::replyFinished(QNetworkReply *reply) 
{ 
    QDesktopServices::openUrl(QUrl("https://qt.io/"));
    Utility::openBrowser("https://qt.io/", nullptr);
}

void getJson::CheckSite(QString url) 
{
  QUrl qrl(url);
  manager = new QNetworkAccessManager(this);
  connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));
  manager->get(QNetworkRequest(qrl));
}