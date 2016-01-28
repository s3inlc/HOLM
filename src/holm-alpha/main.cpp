#include <QCoreApplication>
#include <QList>
#include <iostream>
#include "defines.h"
using namespace std;

void showHelp(){
    //TODO: show help
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
    RunType task = NO_TYPE;
    bool looping = false;
    bool newLists = true;
    int logLevel = 1; // 1 -> normal, 2 -> increased, 3 -> debug
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
            if(logLevel < 1 || logLevel > 3){
                cout << "Invalid verbose level!" << endl << endl;
                showHelp();
                return 0;
            }
        }
        else{
            config.append(argv[x]);
        }
    }

    //TODO: check here, if there is a config with an API key, if not, ask for one and check it then.

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
        //TODO: call single generation here
    }
    else if(config.at(0).compare("single") == 0){
        //execute a single task
        task = SINGLE_TASK;
        if(config.size() < 2){
            cout << "I need a task file to run!" << endl << endl;
            showHelp();
            return 0;
        }
        //TODO: call task execution here with config.at(1)
    }
    else if(config.at(0).compare("multi") == 0){
        //execute all tasks within the task directory
        task = MULTI_TASK;
        //TODO: call multiple task execution
    }

    QCoreApplication a(argc, argv);
    return a.exec();
}












