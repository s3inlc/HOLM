/*
* Project created by QtCreator 2016-01-28T16:04:57
* Copyright 2016 by Sein Coray
*/

#include <QCoreApplication>
#include <QList>
#include <iostream>
#include <QSsl>
#include <QSslSocket>
#include <QDir>
#include <sys/stat.h>
#include <QDir>
#include "defines.h"
#include "logger.h"
#include "taskparser.h"
#include "apimanager.h"
#include "generator.h"
#include "executor.h"
#include "fileparser.h"
#include "uploader.h"
#include "multitask.h"
using namespace std;

void showHelp(){
    //show help
    cout << "HOLM-alpha " << HOLM_VERSION << endl;
    cout << "holm-alpha [gen|single|multi] <options> (<lists|tasks>)" << endl << endl;
    cout << "    gen     generate one or multiple lists from Hashes.org." << endl;
    cout << "            list names are provided with their names like '32', '40' or 'joomla'" << endl;
    cout << "    single  execute a single task given by the task filename" << endl;
    cout << "    multi   execute multiple task files, given as list of argument or all" << endl;
    cout << "            files which are present in the 'tasks' folder" << endl << endl;
    cout << "  --loop    when finished, HOLM will start again from the beginning" << endl;
    cout << "            works for 'single' and 'multi'" << endl;
    cout << "  --upload  automatically upload found hashes to Hashes.org after a task run" << endl;
    cout << "  --old     instead of generating the new left lists, the old left are generated" << endl;
    cout << "   -v [lev] set the logging level (0 is normal, 2 highest)" << endl << endl;
}

bool isValidList(QString name){
    name = name.toLower();
    //check the salted
    if(name.compare("joomla") == 0 || name.compare("oscommerce") == 0 || name.compare("ipb") == 0){
        return true;
    }
    else if(name.compare("mybb") == 0 || name.compare("vbulletin") == 0 || name.compare("joomla") == 0){
        return true;
    }
    else if(name.compare("unknown") == 0 || name.compare("8") == 0 || name.compare("16") == 0){
        return true;
    }
    else if(name.compare("32") == 0 || name.compare("40") == 0 || name.compare("48") == 0){
        return true;
    }
    else if(name.compare("56") == 0 || name.compare("64") == 0 || name.compare("80") == 0){
        return true;
    }
    else if(name.compare("96") == 0 || name.compare("128") == 0){
        return true;
    }
    return false;
}

int main(int argc, char *argv[]){
    cout << "HOLM-alpha starting..." << endl;
    QCoreApplication a(argc, argv);

    //check if folders exist (tasks, data)
    QDir taskDir(TASKS);
    if(!taskDir.exists()){
#ifdef WIN32
        mkdir(TASKS);
#else
        mkdir(TASKS, 0777);
#endif
    }
    QDir dataDir(DATA);
    if(!dataDir.exists()){
#ifdef WIN32
        mkdir(DATA);
#else
        mkdir(DATA, 0777);
#endif
    }

    //check here if SSL is available, if not, abort
    if(!QSslSocket::supportsSsl()){
        cout << "HTTPS libs are not available, required to run!" << endl;
        return -1;
    }

    RunType task = NO_TYPE;
    bool looping = false;
    bool newLists = true;
    bool uploading = false;
    int logLevel = 0; // 0 -> normal, 1 -> increased, 2 -> debug
    QList<QString> config;
    for(int x=1;x<argc;x++){
        //filter out the global configurations
        if(strcmp(argv[x], "--loop") == 0){
            looping = true;
        }
        else if(strcmp(argv[x], "--old") == 0){
            newLists = false;
        }
        else if(strcmp(argv[x], "--upload") == 0){
            uploading = true;
        }
        else if(strcmp(argv[x], "-v") == 0){
            if(x + 1 >= argc){
                cout << "-v requires a verbosity level number!" << endl << endl;
                showHelp();
                return 0;
            }
            logLevel = atoi(argv[x + 1]);
            if(logLevel < 0 || logLevel > 2){
                cout << "Invalid verbose level!" << endl << endl;
                showHelp();
                return 0;
            }
            x++;
        }
        else{
            config.append(argv[x]);
        }
    }
    Logger::setLevel((LogLevel)logLevel);

    //check here, if there is a config with an API key, if not, ask for one and check it then.
    ApiManager apiManager;
    while(!apiManager.validApiAvailable()){
        //get api from console
        cout << "You have not set up an API key, or your entered API key is invalid!" << endl << "Please enter your API key here:" << endl;
        string in;
        cin >> in;
        if(in.length() > 0){
            apiManager.setKey(in.c_str());
        }
    }

    Generator gen;
    FileParser parser;
    Executor exec;
    Uploader uploader;
    MultiTask multi;

    //parse arguments
    if(config.size() < 1){
        cout << "Invalid number of arguments!" << endl << endl;
        showHelp();
        return 0;
    }
    else if(config.at(0).compare("gen") == 0){
        //just generate some left lists
        task = GENERATE;
        if(config.size() < 2){
            cout << "Generation requires at least one more argument!" << endl << endl;
            showHelp();
            return 0;
        }
        QList<QString> toGenerate;
        for(int x=1;x<config.size();x++){
            toGenerate.append(config.at(x));
        }
        //call single generation here
        gen.setLists(toGenerate, newLists);
        gen.start();
        QObject::connect(&gen, SIGNAL(finished()), &a, SLOT(quit()));
    }
    else if(config.at(0).compare("single") == 0){
        //execute a single task
        task = SINGLE_TASK;
        if(config.size() < 2){
            cout << "I need a task file to run!" << endl << endl;
            showHelp();
            return 0;
        }
        //call task execution here with config.at(1)
        parser.parseFile(config.at(1));
        if(!parser.isValid()){
            Logger::log("Abort task execution due to error!", NORMAL);
            return -1;
        }
        QStringList set;
        set.append(parser.getList());
        gen.setLists(set, parser.isNewList());
        exec.setCallString(parser.getCallString());
        QObject::connect(&gen, SIGNAL(finished()), &exec, SLOT(start()));
        if(!looping){
            if(uploading){
                uploader.setConfiguration(parser.getConfiguration());
                QObject::connect(&exec, SIGNAL(finished()), &uploader, SLOT(doUpload()));
                QObject::connect(&uploader, SIGNAL(finished()), &a, SLOT(quit()));
            }
            else{
                QObject::connect(&exec, SIGNAL(finished()), &a, SLOT(quit()));
            }
        }
        else{
            //when looping, just start again with generation
            if(uploading){
                uploader.setConfiguration(parser.getConfiguration());
                QObject::connect(&exec, SIGNAL(finished()), &uploader, SLOT(doUpload()));
                QObject::connect(&uploader, SIGNAL(finished()), &gen, SLOT(start()));
            }
            else{
                QObject::connect(&exec, SIGNAL(finished()), &gen, SLOT(start()));
            }
        }
        Logger::log("Start single task...", NORMAL);
        gen.start();
    }
    else if(config.at(0).compare("multi") == 0){
        //execute all tasks within the task directory
        task = MULTI_TASK;
        QStringList tasks;
        if(config.size() > 1){
            for(int x=1;x<config.size();x++){
                QFileInfo info(config.at(x));
                if(info.isDir()){
                    QDir recoredDir(config.at(x));
                    QStringList filter;
                    filter.append("*.ini");
                    QStringList list = recoredDir.entryList(filter);
                    for(int y=0;y<list.size();y++){
                        //tasks.append();
                        tasks.append(config.at(x) + "/" + list.at(y));
                    }
                }
                else{
                    tasks.append(config.at(x));
                }
            }
        }
        else{
            //scan task folder for files
            QDir recoredDir(TASKS);
            QStringList filter;
            filter.append("*.ini");
            tasks = recoredDir.entryList(filter);
        }
        tasks.removeDuplicates();
        Logger::log("Loaded " + QString::number(tasks.size()) + " tasks.", NORMAL);
        for(int x=0;x<tasks.size();x++){
            Logger::log("Task file: " + tasks.at(x), DEBUG);
        }
        //call multiple task execution
        if(looping){
            QObject::connect(&multi, SIGNAL(allTasksDone()), &multi, SLOT(nextTask()));
        }
        else{
            QObject::connect(&multi, SIGNAL(allTasksDone()), &a, SLOT(quit()));
        }
        if(tasks.size() > 0){
            multi.setTasks(tasks, uploading);
            multi.nextTask();
        }
        else{
            return 0;
        }
    }
    else{
        showHelp();
        return 0;
    }

    return a.exec();
}












