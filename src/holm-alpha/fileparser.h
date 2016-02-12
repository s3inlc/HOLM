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
