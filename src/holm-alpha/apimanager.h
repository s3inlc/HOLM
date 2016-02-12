/*
* Project created by QtCreator 2016-01-28T16:04:57
* Copyright 2016 by Sein Coray
*/

#ifndef APIMANAGER_H
#define APIMANAGER_H

#include <QObject>
#include <QFile>
#include <QTextStream>
#include <QNetworkAccessManager>
#include <QEventLoop>
#include <QUrl>
#include <QNetworkReply>
#include <QObject>
#include "logger.h"
#include "defines.h"

class ApiManager : public QObject
{
    Q_OBJECT
public:
    explicit ApiManager(QObject *parent = 0);
    bool validApiAvailable();
    QString getKey();
    void setKey(QString k);

signals:

public slots:

private:
    QString apiKey;
    bool isValid;
};

#endif // APIMANAGER_H
