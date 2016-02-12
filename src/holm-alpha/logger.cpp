/*
* Project created by QtCreator 2016-01-28T16:04:57
* Copyright 2016 by Sein Coray
*/

#include "logger.h"

LogLevel Logger::level(NORMAL);

void Logger::log(QString msg, LogLevel lvl){
    if(lvl <= level){
        cout << "LOG: " << msg.toStdString() << endl;
    }
}

void Logger::setLevel(LogLevel lvl){
    level = lvl;
}

LogLevel Logger::getLevel(){
    return level;
}
