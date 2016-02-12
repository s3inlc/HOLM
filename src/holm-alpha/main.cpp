#include <QCoreApplication>
#include <QList>
#include <iostream>
#include <QSsl>
#include <QSslSocket>
#include <QDir>
#include <sys/stat.h>
#include "defines.h"
#include "logger.h"
#include "taskparser.h"
#include "apimanager.h"
#include "generator.h"
#include "executor.h"
#include "fileparser.h"
using namespace std;

void showHelp(){
    //TODO: show help
    cout << "HOLM-alpha " << HOLM_VERSION << endl;
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
        QStringList set;
        set.append(parser.getList());
        gen.setLists(set, parser.isNewList());
        exec.setCallString(parser.getCallString());
        QObject::connect(&gen, SIGNAL(finished()), &exec, SLOT(start()));
        QObject::connect(&exec, SIGNAL(finished()), &a, SLOT(quit()));
    }
    else if(config.at(0).compare("multi") == 0){
        //execute all tasks within the task directory
        task = MULTI_TASK;
        //TODO: call multiple task execution
    }
    else{
        showHelp();
        return 0;
    }

    return a.exec();
}












