/*
* Project created by QtCreator 2016-01-28T16:04:57
* Copyright 2016 by Sein Coray
*/

#ifndef TASKPARSER_H
#define TASKPARSER_H

#include <QObject>
#include <QFile>
#include <QTextStream>
#include "defines.h"
#include "logger.h"

class TaskParser : public QObject{
    Q_OBJECT
public:
    TaskParser(QString path);
    ~TaskParser();
    bool isValid();
    Configuration getConfig();

signals:

public slots:

private:
    QString errMessage;
    Configuration config;
    bool valid;
};

#endif // TASKPARSER_H
