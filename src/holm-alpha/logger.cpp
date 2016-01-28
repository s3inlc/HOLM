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
