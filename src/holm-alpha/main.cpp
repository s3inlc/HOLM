#include <QCoreApplication>
#include <iostream>
#include "defines.h"
using namespace std;

void showHelp(){
    //TODO: show help
}

int main(int argc, char *argv[]){
    RunType task = NO_TYPE;
    bool looping = false;

    //parse arguments
    if(argc < 2){
        cout << "Invalid number of arguments!" << endl << endl;
        showHelp();
        return 0;
    }
    else if(strcmp(argv[1], "gen") == 0){
        //just generate some left lists
        task = GENERATE;
        if(argc < 3){
            cout << "Generation requires at least one more argument!" << endl << endl;
            showHelp();
            return 0;
        }
    }
    else if(strcmp(argv[1], "single") == 0){
        //execute a single task
        task = SINGLE_TASK;
        if(argc < 3){
            cout << "I need a task file to run!" << endl << endl;
            showHelp();
            return 0;
        }
    }
    else if(strcmp(argv[1], "multi") == 0){
        //execute all tasks within the task directory
        task = MULTI_TASK;
    }

    QCoreApplication a(argc, argv);
    return a.exec();
}












