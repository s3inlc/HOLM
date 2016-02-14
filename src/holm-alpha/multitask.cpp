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
    QObject::connect(&gen, SIGNAL(finished()), &exec, SLOT(start()));
    if(uploading){
        QObject::connect(&exec, SIGNAL(finished()), &uploader, SLOT(doUpload()));
        QObject::connect(&uploader, SIGNAL(finished()), this, SLOT(taskFinished()));
    }
    else{
        QObject::connect(&exec, SIGNAL(finished()), this, SLOT(taskFinished()));
    }
}

void MultiTask::taskFinished(){
    taskPos++;
    if(taskPos >= tasks.size()){
        taskPos = 0;
        emit allTasksDone();
        return;
    }
    nextTask();
}

void MultiTask::nextTask(){
    if(taskPos >= tasks.size()){
        taskFinished();
        return;
    }
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
    if(uploading){
        uploader.setConfiguration(parser.getConfiguration());
    }
    Logger::log("Start task '" + tasks.at(taskPos) + "'...", NORMAL);
    gen.start();
}



