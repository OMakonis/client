#ifndef GETJSON_H
#define GETJSON_H

#include <QObject>

#include <QtCore>
#include <QNetworkAccessManager>
#include <QNetworkReply>

class getJson : public QObject
{
Q_OBJECT
  QNetworkAccessManager *manager;
private slots:
  void replyFinished(QNetworkReply *);
public:
  void CheckSite(QString url);
};



#endif // GETJSON_H
