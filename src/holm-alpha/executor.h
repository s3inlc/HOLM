/*
* Project created by QtCreator 2016-01-28T16:04:57
* Copyright 2016 by Sein Coray
*/

#ifndef EXECUTOR_H
#define EXECUTOR_H

#include <QThread>
#include "logger.h"

class Executor : public QThread{
    Q_OBJECT
public:
    explicit Executor(QObject *parent = 0);
    ~Executor();
    void setCallString(QString call);

signals:

public slots:

private:
    void run();
    QString callString;
};

#endif // EXECUTOR_H
