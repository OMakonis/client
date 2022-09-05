#include "getJson.h"
#include "guiutility.h"

#include <QApplication>
#include <QDesktopServices>
#include <QUrl>
#include <QClipboard>


void getJson::replyFinished(QNetworkReply *reply) 
{ 
  QApplication::clipboard()->setText("https://failiem.lv/server_scripts/filesfm_sync_contextmenu_action.php?username=demo&path=/test_folder1/test_folder2/&action=get_share_link");
}

void getJson::CheckSite(QString url) 
{
  QApplication::clipboard()->setText("https://www.youtube.com/");
  QUrl qrl(url);
  manager = new QNetworkAccessManager(this);
  connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));
  manager->get(QNetworkRequest(qrl));
}