#include "generator.h"

Generator::Generator(QObject *parent) : QThread(parent){
    lists.clear();
    newLists = true;
    downloadManager = new QNetworkAccessManager();
    connect(this, SIGNAL(triggerDownloadFile(QString,bool)), this, SLOT(downloadFile(QString,bool)));
    connect(this, SIGNAL(triggerCheckChecksum(QString,bool)), this, SLOT(checkChecksum(QString,bool)));
    connect(this, SIGNAL(triggerGetIdentifiers()), this, SLOT(getIdentifiers()));
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
            emit triggerCheckChecksum(lists.at(x), newLists);
            checking = true;
            while(checking){
                usleep(100); //waiting for the check
            }
            emit triggerGetIdentifiers();
            checking = true;
            while(checking){
                usleep(100); //wait for loading the identifiers
            }
            emit triggerCreateList(lists.at(x), newLists);
            generating = true;
            while(generating){
                usleep(100); //waiting for the generation
            }
        }
        else{
            emit triggerDownloadFile(lists.at(x), newLists);
            downloading = true;
            while(downloading){
                usleep(100); //waiting for the download
            }
            emit triggerGetIdentifiers();
            checking = true;
            while(checking){
                usleep(100); //wait for loading the identifiers
            }
            emit triggerCreateList(lists.at(x), newLists);
            generating = true;
            while(generating){
                usleep(100); //waiting for the generation
            }
        }
    }
    delete downloadManager;
}

void Generator::createList(QString name, bool newLists){
    //check for checksum

    //load identifiers

    //create file
}

void Generator::downloadFile(QString name, bool newLists){
    downloading = true;
    lastPercent = 0;
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
    currentPath = DATA + QString("/") + name + "_" + type + ".txt";
    QFile file(currentPath);
    file.remove(); //delete any other files
    Logger::log("Loading: " + file.fileName() + " - " + url.toString(), NORMAL);
    QNetworkRequest request(url);
    downloadReply = downloadManager->get(request);
    QObject::connect(downloadReply, SIGNAL(readyRead()), this, SLOT(downloadRead()));
    QObject::connect(downloadReply, SIGNAL(downloadProgress(qint64,qint64)), this, SLOT(updateDownloadProgress(qint64, qint64)));
    QObject::connect(downloadReply, SIGNAL(finished()), this, SLOT(downloadRead()));
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

void Generator::checkChecksum(QString name, bool newLists){
    checking = true;
    QNetworkAccessManager manager(this);
    QEventLoop eventLoop;
    QString path = DATA + QString("/") + name;
    if(newLists){
        path += "_new.txt";
    }
    else{
        path += "_old.txt";
    }
    connect(&manager, SIGNAL(finished(QNetworkReply*)), &eventLoop, SLOT(quit()));
    Logger::log("Check checksum...", INCREASED);
    QString hashCode = byteToStr(fileChecksum(path, QCryptographicHash::Sha1));
    cout << hashCode.toStdString() << endl;
    QUrl url(QString(API) + "?holm=sumcheck&sum=" + hashCode);
    Logger::log("CALL: " + url.toString(), DEBUG);
    QNetworkReply *reply = manager.get(QNetworkRequest(url));
    eventLoop.exec(); //waiting..
    QString data;
    if(reply->error() == QNetworkReply::NoError){
        //success
        data = reply->readAll();
        if(data.compare("NEW") == 0){
            //file needs to be downloaded!
            Logger::log("Current list is not identified, need to load again!", INCREASED);
            downloading = true;
            while(downloading){
                usleep(100); //waiting for the download
            }
            emit triggerCheckChecksum(name, newLists); // I know this is bad at the moment. In case of bugs it can cause looping
            checking = true;
            while(checking){
                usleep(100); //waiting for the check
            }
        }
        else{
            //we get the identifiers
            currentIdentifiers = data.split(";");
            Logger::log("Got " + QString::number(currentIdentifiers.size()) + " identifiers for list " + name, NORMAL);
        }
    }
    else{
        //failure
        Logger::log("Checksum request failed with: " + reply->errorString(), NORMAL);
    }
    delete reply;
    checking = false;
}

void Generator::getIdentifiers(){
    checking = true;
    QNetworkAccessManager manager(this);
    QEventLoop eventLoop;
    connect(&manager, SIGNAL(finished(QNetworkReply*)), &eventLoop, SLOT(quit()));
    for(int x=0;x<currentIdentifiers.size();x++){
        QUrl url(QString(API) + "?holm=get&key=" + currentIdentifiers.at(x));
        Logger::log("Loading identifier " + currentIdentifiers.at(x) + "!", INCREASED);
        Logger::log("CALL: " + url.toString(), DEBUG);
        QNetworkReply *reply = manager.get(QNetworkRequest(url));
        eventLoop.exec(); //waiting..
        QString data;
        if(reply->error() == QNetworkReply::NoError){
            //success
            data = reply->readAll();
            if(data.compare("ERROR") == 0){
                Logger::log("Error with loading '" + currentIdentifiers.at(x) + "'!", NORMAL);
            }
            else{
                Logger::log("Received identifier '" + currentIdentifiers.at(x) + "'", DEBUG);
                //TODO: save identifier here
            }
        }
        else{
            //failure
            Logger::log("Identifier request failed with: " + reply->errorString(), NORMAL);
        }
        delete reply;
    }
    checking = false;
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

QString Generator::byteToStr(QByteArray arr){
    string res = "";
    for(int x=0;x<arr.length();x++){
        UINT8 u = arr.at(x);
        unsigned int r = u;
        stringstream s;
        if(r<16){
            s << "0";
        }
        s << std::hex << r;
        res += s.str().c_str();
    }
    return QString(res.c_str());
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

