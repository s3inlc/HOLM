#ifndef EXECUTOR_H
#define EXECUTOR_H

#include <QThread>

class Executor : public QThread{
    Q_OBJECT
public:
    explicit Executor(QObject *parent = 0);
    ~Executor();
    void setCallString(QString call);

signals:

public slots:

private:
    void run();
};

#endif // EXECUTOR_H
