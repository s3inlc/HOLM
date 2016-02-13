/*
* Project created by QtCreator 2016-01-28T16:04:57
* Copyright 2016 by Sein Coray
*/

#include "executor.h"

Executor::Executor(QObject *parent) : QThread(parent){
    //
}

Executor::~Executor(){
    //
}

void Executor::setCallString(QString call){
    callString = call;
}

void Executor::run(){
    Logger::log("Executing: " + callString, DEBUG);
    system(callString.toStdString().c_str());
    Logger::log("Hashcracking run is finished!", NORMAL);
}
