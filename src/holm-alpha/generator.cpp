#include "generator.h"

Generator::Generator(QObject *parent) : QThread(parent){
    lists.clear();
    newLists = true;
    downloadManager = new QNetworkAccessManager();
    connect(this, SIGNAL(triggerDownloadFile(QString,bool)), this, SLOT(downloadFile(QString,bool)));
    connect(this, SIGNAL(triggerCheckChecksum(QString,bool)), this, SLOT(checkChecksum(QString,bool)));
    connect(this, SIGNAL(triggerGetIdentifiers()), this, SLOT(getIdentifiers()));
    connect(this, SIGNAL(triggerCreateList(QString,bool)), this, SLOT(createList(QString,bool)));
}

void Generator::setLists(QList<QString> list, bool listType){
    Logger::log("Configured Generator to load " + QString::number(list.size()) + " lists!", DEBUG);
    lists = list;
    newLists = listType;
}

Generator::~Generator(){
    delete downloadManager;
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
        Logger::log("Finished " + lists.at(x) + "!", NORMAL);
    }
    Logger::log("Finished all list generations!", NORMAL);
}

void Generator::createList(QString name, bool newLists){
    //load all identifiers into RAM
    generating = true;
    QMap<QString,bool> data;
    Logger::log("Start list creation for " + name + "...", INCREASED);
    Logger::log("Load all identifiers...", NORMAL);
    for(int x=0;x<currentIdentifiers.size();x++){
        //get identifier file if existing
        QString identifierPath = DATA + QString("/") + currentIdentifiers.at(x) + ".txt";
        QFile id(identifierPath);
        if(!id.exists()){
            //if it doesn't exist, we will ignore it
            Logger::log("Ignore " + currentIdentifiers.at(x) + " because it doesn't exist!", INCREASED);
            continue;
        }
        QString idData = fileGetContents(identifierPath);
        QStringList list = idData.replace("\r\n", "\n").split("\n");
        for(int y=0;y<list.size();y++){
            QString hash;
            bool act;
            if(list.at(y).contains(":")){
                //salted list
            }
            else{
                //unsalted hash
                hash = list.at(y).mid(1);
                if(hash.length() != name.toInt()){
                    continue;
                }
                if(list.at(y).at(0) == '+'){
                    act = true;
                }
                else{
                    act = false;
                }
                if(!data.keys().contains(hash)){
                    data.insert(hash, act);
                }
                else if(data.value(hash) && !act){
                    data.remove(hash);
                }
                else if(!data.value(hash) && act){
                    data.remove(hash);
                }
            }
        }
    }
    Logger::log("Loaded " + QString::number(data.size()) + " hash entries!", NORMAL);

    //open list and file path to write to
    QString outputPath = name + "_";
    QString inputPath = DATA + QString("/") + name + "_";
    if(newLists){
        outputPath += "new.txt";
        inputPath += "new.txt";
    }
    else{
        outputPath += "old.txt";
        inputPath += "old.txt";
    }
    QFile output(outputPath);
    QFile input(inputPath);
    if(!output.open(QIODevice::WriteOnly)){
        Logger::log("Failed to open output file!", NORMAL);
        generating = false;
        return;
    }
    if(!input.open(QIODevice::ReadOnly)){
        Logger::log("Failed to open input list file!", NORMAL);
        generating = false;
        return;
    }

    int delCount = 0;
    int addCount = 0;

    //go trough list and remove/add hashes
    while(!input.atEnd()){
        QString line = input.readLine().replace("\n", "");
        if(line.length() == 0){
            continue;
        }
        if(data.contains(line) && !data.value(line)){
            delCount++;
            continue;
        }
        else if(data.contains(line)){
            data.remove(line);
        }
        output.write(line.toUtf8() + "\n");
    }

    input.close();

    //add hashes which are not handled currently
    for(int x=0;x<data.size();x++){
        if(!data.values().at(x)){
            continue;
        }
        addCount++;
        output.write(data.keys().at(x).toUtf8() + "\n");
    }
    output.close();

    Logger::log("Added " + QString::number(addCount) + " hashes and removed " + QString::number(delCount) + " hashes from " + name, NORMAL);

    generating = false;
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
    ApiManager apiManager;
    QUrl url(QString(API) + "?holm=sumcheck&key=" + apiManager.getKey() + "&sum=" + hashCode);
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
            emit triggerDownloadFile(name, newLists);
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
    ApiManager apiManager;
    connect(&manager, SIGNAL(finished(QNetworkReply*)), &eventLoop, SLOT(quit()));
    for(int x=0;x<currentIdentifiers.size();x++){
        QFile file(DATA + QString("/") + currentIdentifiers.at(x) + ".txt");
        if(file.exists()){
            continue;
        }
        QUrl url(QString(API) + "?holm=get&key=" + apiManager.getKey() + "&id=" + currentIdentifiers.at(x));
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
                //save identifier here
                if(file.open(QIODevice::WriteOnly)){
                    QTextStream stream(&file);
                    stream << data;
                }
                file.close();
            }
        }
        else{
            //failure
            Logger::log("Identifier request failed with: " + reply->errorString(), NORMAL);
        }
        delete reply;
    }
    Logger::log("All required identifiers are loaded and/or saved!", NORMAL);
    checking = false;
}

void Generator::updateDownloadProgress(qint64 prog, qint64 tot){
    int percent = (int)(prog*100/tot);
    if(Logger::getLevel() == NORMAL){
        if(percent == lastPercent || tot == -1){
            return;
        }
        lastPercent = percent;
        Logger::log("Progress: " + QString::number(percent) + "%", NORMAL);
    }
    else{
        if(tot == -1){
            Logger::log("Progress: " + QString::number(prog) + " Bytes", INCREASED);
        }
        else{
            Logger::log("Progress: " + QString::number(prog) + "/" + QString::number(tot) + " (" + QString::number(percent) + "%)", INCREASED);
        }
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

QString Generator::fileGetContents(QString path){
    QFile f(path);
    if(!f.open(QFile::ReadOnly)){
        Logger::log("Failed to open file: '" + path + "!", INCREASED);
        return "";
    }
    QTextStream in(&f);
    return in.readAll();
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

