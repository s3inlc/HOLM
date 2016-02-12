/*
* Project created by QtCreator 2016-01-28T16:04:57
* Copyright 2016 by Sein Coray
*/

#ifndef FILEPARSER_H
#define FILEPARSER_H

#include <QObject>

class FileParser : public QObject
{
    Q_OBJECT
public:
    explicit FileParser(QObject *parent = 0);
    ~FileParser();
    void parseFile(QString task);
    QString getCallString();
    QString getList();
    bool isNewList();

signals:

public slots:
};

#endif // FILEPARSER_H
