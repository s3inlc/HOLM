#include "generator.h"

Generator::Generator(QObject *parent) : QThread(parent){
    lists.clear();
    newLists = true;
}

void Generator::setLists(QList<QString> list, bool listType){
    Logger::log("Configured Generator to load " + QString::number(list.size()) + " lists!", DEBUG);
    lists = list;
    newLists = listType;
}

Generator::~Generator(){
    //
}

void Generator::run(){
    if(lists.size() == 0){
        Logger::log("No lists to create! Finishing...", NORMAL);
        return;
    }

    //do job for every list
    QString type = "new";
    if(!newLists){
        type = "old";
    }
    for(int x=0;x<lists.size();x++){
        downloading = false;
        Logger::log("Processing " + lists.at(x) + "...", NORMAL);
        //check if there is a list present in the data directory
        QFile list(DATA + QString("/" + lists.at(x) + "_" + type + ".txt"));
        if(list.exists()){
            createList(lists.at(x), newLists);
        }
        else{
            downloadFile(lists.at(x), newLists);
            while(downloading){
                usleep(100);
            }
            createList(lists.at(x), newLists);
        }
    }
}

void Generator::createList(QString name, bool newLists){
    //check for checksum

    //load identifiers

    //create file
}

void Generator::downloadFile(QString name, bool newLists){
    downloading = true;
    lastPercent = 0;
    currentPath = DATA + QString("/") + name + ".txt";
    QUrl url;
    QString type = "new";
    if(!newLists){
        type = "old";
    }
    if(name.toInt() == 0){
        //salted list
        url = QUrl(DL_PATH + QString("?type=salted&algorithm=" + name + "&format=plain&time=" + type));
    }
    else{
        //unsalted list
        url = QUrl(DL_PATH + QString("?type=left&len=" + name + "&format=plain&time=" + type));
    }
    QFile file(currentPath);
    file.remove(); //delete any other files
    Logger::log("Loading: " + file.fileName(), NORMAL);
    QNetworkRequest request(url);
    downloadReply = downloadManager.get(request);
    connect(downloadReply, SIGNAL(readyRead()), this, SLOT(downloadRead()));
    connect(downloadReply, SIGNAL(downloadProgress(qint64,qint64)), this, SLOT(updateDownloadProgress(qint64, qint64)));
    connect(downloadReply, SIGNAL(finished()), this, SLOT(downloadRead()));
}

void Generator::downloadRead(){
    if(!downloading){
        downloadReply->abort();
        return;
    }
    else if(downloadReply->isFinished()){
        Logger::log("Finished download!", NORMAL);
        downloading = false;
        delete downloadReply;
        return;
    }
    else if(downloadReply->bytesAvailable() <= 0){
        return;
    }
    QByteArray data = downloadReply->readAll();
    QFile file(currentPath);
    file.open(QIODevice::WriteOnly | QIODevice::Append);
    file.write(data);
    file.close();
}

void Generator::updateDownloadProgress(qint64 prog, qint64 tot){
    int percent = (int)(prog*100/tot);
    if(Logger::getLevel() == NORMAL){
        if(percent == lastPercent){
            return;
        }
        Logger::log("Progress: " + QString::number(percent) + "%", NORMAL);
    }
    else{
        Logger::log("Progress: " + QString::number(prog) + "/" + QString::number(tot) + " (" + QString::number(percent) + "%)", INCREASED);
    }
}

QByteArray Generator::fileChecksum(const QString &fileName, QCryptographicHash::Algorithm hashAlgorithm){
    QFile f(fileName);
    if (f.open(QFile::ReadOnly)) {
        QCryptographicHash hash(hashAlgorithm);
        if (hash.addData(&f)) {
            return hash.result();
        }
    }
    return QByteArray();
}

