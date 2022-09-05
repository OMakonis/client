#include "getJson.h"
#include "guiutility.h"

#include <QApplication>
#include <QDesktopServices>
#include <QUrl>
#include <QClipboard>


void getJson::replyFinished(QNetworkReply *reply) 
{ 
    QApplication::clipboard()->setText("https://qt.io/");
}

void getJson::CheckSite(QString url) 
{
  QUrl qrl(url);
  manager = new QNetworkAccessManager(this);
  connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));
  manager->get(QNetworkRequest(qrl));
}