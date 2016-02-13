#ifndef MULTITASK_H
#define MULTITASK_H

#include <QObject>
#include "executor.h"
#include "defines.h"
#include "logger.h"
#include "generator.h"
#include "fileparser.h"
#include "taskparser.h"
#include "uploader.h"

class MultiTask : public QObject
{
    Q_OBJECT
public:
    explicit MultiTask(QObject *parent = 0);
    ~MultiTask();
    void setTasks(QStringList t, bool up);

signals:
    void allTasksDone();

public slots:
    void nextTask();
    void taskFinished();

private:
    QStringList tasks;
    int taskPos;
    bool uploading;
    Executor exec;
    Generator gen;
    FileParser parser;
    Uploader uploader;
};

#endif // MULTITASK_H
