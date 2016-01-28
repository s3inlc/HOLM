#include "taskparser.h"

TaskParser::TaskParser(QString path) : QObject(){
    //TODO: parse given file path here
    valid = true;
}

TaskParser::~TaskParser(){
    //
}

bool TaskParser::isValid(){
    return valid;
}

