#ifndef APIMANAGER_H
#define APIMANAGER_H

#include <QObject>

class ApiManager : public QObject
{
    Q_OBJECT
public:
    explicit ApiManager(QObject *parent = 0);
    ~ApiManager();

signals:

public slots:
};

#endif // APIMANAGER_H
