#ifndef GENERATOR_H
#define GENERATOR_H

#include <QObject>
#include <QThread>
#include <QList>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QFile>
#include <sstream>
#include "logger.h"
#include "apimanager.h"

class Generator : public QThread{
    Q_OBJECT
public:
    explicit Generator(QObject *parent = 0);
    void setLists(QList<QString> list, bool listType);
    static QByteArray fileChecksum(const QString &fileName, QCryptographicHash::Algorithm hashAlgorithm);
    static QString byteToStr(QByteArray arr);
    static QString fileGetContents(QString path);
    ~Generator();

signals:
    void triggerDownloadFile(QString name, bool newLists);
    void triggerCheckChecksum(QString name, bool newLists);
    void triggerCreateList(QString name, bool newLists);
    void triggerGetIdentifiers();

public slots:
    void downloadFile(QString name, bool newLists);
    void updateDownloadProgress(qint64 prog, qint64 tot);
    void downloadRead();
    void checkChecksum(QString name, bool newLists);
    void getIdentifiers();
    void createList(QString name, bool newLists);

private:
    void run();
    QList<QString> lists;
    QList<QString> currentIdentifiers;
    bool newLists, downloading, generating, checking;
    QString currentPath;
    int lastPercent;
    QNetworkAccessManager *downloadManager;
    QNetworkReply *downloadReply;
};

#endif // GENERATOR_H
