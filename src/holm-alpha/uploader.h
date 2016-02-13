#ifndef UPLOADER_H
#define UPLOADER_H

#include <QObject>
#include <QThread>
#include <QTextStream>
#include <QFile>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QEventLoop>
#include "apimanager.h"
#include "logger.h"
#include "defines.h"

class Uploader : public QObject{
    Q_OBJECT
public:
    explicit Uploader(QObject *parent = 0);
    ~Uploader();
    void setConfiguration(Configuration conf);

signals:
    void finished();

public slots:
    void doUpload();

private:
    Configuration config;
    QNetworkAccessManager *manager;
    ApiManager apiManager;
};

#endif // UPLOADER_H
