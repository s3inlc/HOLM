/*
* Project created by QtCreator 2016-01-28T16:04:57
* Copyright 2016 by Sein Coray
*/

#include "fileparser.h"

FileParser::FileParser(QObject *parent) : QObject(parent){
    taskIsValid = false;
    config.newList = true;
}

FileParser::~FileParser(){
    //
}

void FileParser::parseFile(QString task){
    config.application = EMPTY;
    QFile file(task);
    if(!file.exists() || !file.open(QIODevice::ReadOnly)){
        Logger::log("Failed to open task file: " + task, NORMAL);
        return;
    }
    QTextStream in(&file);
    QStringList content = in.readAll().replace("\r\n", "\n").split("\n");
    for(int x=0;x<content.size();x++){
        if(content.at(x).at(0) == '#'){
            continue; //ignore comment lines
        }
        QStringList kval = content.at(x).split("=");
        if(kval.size() < 2){
            Logger::log("Invalid task file line: " + content.at(x), INCREASED);
            continue;
        }
        QString key = kval.at(0);
        kval.removeAt(0);
        QString value = kval.join("=");
        if(key.compare("hashcat") == 0){
            config.application = OCLHASHCAT;
            config.binaryPath = value;
        }
        else if(key.compare("mdxfind") == 0){
            config.application = MDXFIND;
            config.binaryPath = value;
        }
        else if(key.compare("algorithm") == 0){
            config.algorithm = value;
        }
        else if(key.compare("list") == 0){
            config.list = value;
        }
        else if(key.compare("type") == 0){
            if(value.compare("old") == 0){
                config.newList = false;
            }
            else{
                config.newList = true;
            }
        }
        else if(key.compare("call") == 0){
            config.call = value;
        }
        else{
            Logger::log("Unknown key setting: " + content.at(x), INCREASED);
        }
    }

    //check if task is valid
    taskIsValid = false;
    if(!isValidList(config.list)){
        Logger::log("Invalid list from task file!", NORMAL);
        return;
    }
    else if(config.application == EMPTY){
        Logger::log("No binary type defined in task file!", NORMAL);
        return;
    }
    int testAlgo = config.algorithm.toInt();
    if(config.application == OCLHASHCAT && testAlgo == 0 && config.algorithm.compare("0") != 0){
        Logger::log("If you use oclHashcat you need to provide the algorithm numerically!", NORMAL);
        return;
    }
    QFile bin(config.binaryPath);
    if(!bin.exists()){
        Logger::log("Binary file to call does not exist: " + config.binaryPath, NORMAL);
        return;
    }
    else if(config.call.length() == 0){
        Logger::log("No call string provided!", NORMAL);
        return;
    }
    taskIsValid = true;
}

QString FileParser::getCallString(){
    QString listPath = config.list;
    if(config.newList){
        listPath += "_new.txt";
    }
    else{
        listPath += "_old.txt";
    }
    QFileInfo list(listPath);
    if(config.application == OCLHASHCAT){
        return config.binaryPath + " -m " + config.algorithm + " -o runHashcat.out '" + list.absoluteFilePath() + "' " + config.call;
    }
    else if(config.application == MDXFIND){
        return config.binaryPath + " -h '" + config.algorithm + "' -f '" + list.absoluteFilePath() + "' " + config.call + " >> runHashcat.out";
    }
    else{
        return "";
    }
}

QString FileParser::getList(){
    return config.list;
}

QString FileParser::getBinaryPath(){
    return config.binaryPath;
}

bool FileParser::isNewList(){
    return config.newList;
}

bool FileParser::isValid(){
    return taskIsValid;
}

