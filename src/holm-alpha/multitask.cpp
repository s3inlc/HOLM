#include "multitask.h"

MultiTask::MultiTask(QObject *parent) : QObject(parent){
    //
}

MultiTask::~MultiTask(){
    //
}

void MultiTask::setTasks(QStringList t, bool up){
    tasks = t;
    taskPos = 0;
    uploading = up;
}

void MultiTask::taskFinished(){
    taskPos++;
    if(taskPos >= tasks.size()){
        emit allTasksDone();
        return;
    }
    nextTask();
}

void MultiTask::nextTask(){
    parser.parseFile(tasks.at(taskPos));
    if(!parser.isValid()){
        Logger::log("Skip task execution due to error!", NORMAL);
        taskFinished();
        return;
    }
    QStringList set;
    set.append(parser.getList());
    gen.setLists(set, parser.isNewList());
    exec.setCallString(parser.getCallString());
    QObject::connect(&gen, SIGNAL(finished()), &exec, SLOT(start()));
    if(uploading){
        uploader.setConfiguration(parser.getConfiguration());
        QObject::connect(&exec, SIGNAL(finished()), &uploader, SLOT(doUpload()));
        QObject::connect(&uploader, SIGNAL(finished()), this, SLOT(taskFinished()));
    }
    else{
        QObject::connect(&exec, SIGNAL(finished()), this, SLOT(taskFinished()));
    }
    Logger::log("Start task '" + tasks.at(taskPos) + "'...", NORMAL);
    gen.start();
}



