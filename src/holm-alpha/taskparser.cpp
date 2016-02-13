/*
* Project created by QtCreator 2016-01-28T16:04:57
* Copyright 2016 by Sein Coray
*/

#include "taskparser.h"

TaskParser::TaskParser(QString path) : QObject(){
    valid = false;
    config.algorithm = -1;
    config.application = EMPTY;
    config.call = "";
    config.newList = true;
    config.binaryPath = "";

    //read file
    Logger::log("Opening task file '" + path + "'...", DEBUG);
    QFile f(path);
    if (!f.open(QFile::ReadOnly | QFile::Text)){
        Logger::log("Failed to load task file!", NORMAL);
        return;
    }
    QTextStream in(&f);
    QStringList content = in.readAll().replace("\r\n", "\n").split("\n");
    f.close();
    for(int x=0;x<content.length();x++){
        if(content.at(x).length() == 0){
            continue; //empty line
        }
        else if(content.at(x).at(0) == '#'){
            continue; //comment line
        }
        QStringList lineSplit = content.at(x).split("=");
        if(lineSplit.size() < 2){
            Logger::log("Invalid line in task file: '" + content.at(x) + "'", INCREASED);
            continue;
        }
        QString key = lineSplit.at(0);
        lineSplit.removeAt(0);
        QString value = lineSplit.join("=");
        if(key.compare("hashcat") == 0){
            config.application = OCLHASHCAT;
            config.binaryPath = value;
        }
        else if(key.compare("algorithm") == 0){
            config.algorithm = value.toInt();
        }
        else if(key.compare("list") == 0){
            config.list = value;
        }
        else if(key.compare("type") == 0){
            if(value.compare("old") == 0){
                config.newList = false;
            }
        }
        else if(key.compare("call") == 0){
            config.call = value;
        }
        else{
            Logger::log("Invalid entry (" + key + ") in task file: '" + content.at(x) + "'", INCREASED);
        }
    }

    //check if config is valid
    if(config.algorithm < 0){
        return;
    }
    else if(config.application == EMPTY){
        return;
    }
    else if(config.call.length() == 0){
        return;
    }
    else if(config.list.length() == 0){
        return;
    }
    valid = true;
}

TaskParser::~TaskParser(){
    //
}

bool TaskParser::isValid(){
    return valid;
}

Configuration TaskParser::getConfig(){
    return config;
}

