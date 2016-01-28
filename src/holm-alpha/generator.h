#ifndef GENERATOR_H
#define GENERATOR_H

#include <QObject>
#include <QThread>
#include <QList>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include "logger.h"
#include "apimanager.h"

class Generator : public QThread{
    Q_OBJECT
public:
    explicit Generator(QObject *parent = 0);
    void setLists(QList<QString> list, bool listType);
    QByteArray fileChecksum(const QString &fileName, QCryptographicHash::Algorithm hashAlgorithm);
    ~Generator();

signals:

public slots:

private:
    void run();
    QList<QString> lists;
    bool newLists, downloading;
    QString currentPath;
    int lastPercent;
    QNetworkAccessManager downloadManager;
    QNetworkReply *downloadReply;
    void createList(QString name, bool newLists);
    void downloadFile(QString name, bool newLists);
    void updateDownloadProgress(qint64 prog, qint64 tot);
    void downloadRead();
};

#endif // GENERATOR_H
