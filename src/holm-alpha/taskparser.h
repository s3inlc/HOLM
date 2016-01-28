#ifndef TASKPARSER_H
#define TASKPARSER_H

#include <QObject>
#include "defines.h"

class TaskParser : public QObject{
    Q_OBJECT
public:
    TaskParser(QString path);
    ~TaskParser();
    bool isValid();

signals:

public slots:

private:
    QString errMessage;
    Configuration config;
    bool valid;
};

#endif // TASKPARSER_H
