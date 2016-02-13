/*
* Project created by QtCreator 2016-01-28T16:04:57
* Copyright 2016 by Sein Coray
*/

#ifndef FILEPARSER_H
#define FILEPARSER_H

#include <QObject>
#include <QFile>
#include <QTextStream>
#include <QFileInfo>
#include "logger.h"
#include "defines.h"

class FileParser : public QObject
{
    Q_OBJECT
public:
    explicit FileParser(QObject *parent = 0);
    ~FileParser();
    void parseFile(QString task);
    QString getCallString();
    QString getList();
    QString getBinaryPath();
    bool isNewList();
    bool isValid();

signals:

public slots:

private:
    bool taskIsValid;
    Configuration config;
};

#endif // FILEPARSER_H
