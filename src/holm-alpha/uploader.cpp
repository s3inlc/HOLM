#include "uploader.h"

Uploader::Uploader(QObject *parent) : QObject(parent){
    manager = new QNetworkAccessManager();
}

Uploader::~Uploader(){
    delete manager;
}

void Uploader::setConfiguration(Configuration conf){
    config = conf;
}

void Uploader::doUpload(){
    if(config.application == OCLHASHCAT){
        QString output = "runHashcat_" + config.algorithm + ".out";
        QFile outputFile(output);
        if(!outputFile.exists()){
            Logger::log("Output file does not exist!", NORMAL);
            emit finished();
            return;
        }
        else if(!outputFile.open(QIODevice::ReadOnly)){
            Logger::log("Failed to open output file!", NORMAL);
            emit finished();
            return;
        }
        QTextStream in(&outputFile);
        QString data = in.readAll();
        outputFile.close();
        if(data.length() == 0){
            Logger::log("Output file is empty!", INCREASED);
            outputFile.remove();
            emit finished();
            return;
        }
        if(config.list.toInt() != 0){
            QStringList conv;
            QStringList list = data.replace("\r\n", "\n").split("\n");
            for(int x=0;x<list.size();x++){
                if(list.at(x).length() == 0){
                    continue;
                }
                QStringList split = list.at(x).split(":");
                split.removeAt(0);
                conv.append(split.join(":"));
            }
            data = conv.join("\n");
        }
        else{
            data = data.replace("\r\n", "\n");
        }
        QEventLoop eventLoop;
        connect(manager, SIGNAL(finished(QNetworkReply*)), &eventLoop, SLOT(quit()));
        QUrl url(QString(API) + "?holm=upload&key=" + apiManager.getKey());
        Logger::log("CALL: " + url.toString(), DEBUG);
        QByteArray callData = QString("algorithm=" + config.algorithm).toUtf8();
        callData += "&data=" + data;
        QNetworkRequest req = QNetworkRequest(url);
        req.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
        QNetworkReply *reply = manager->post(req, callData);
        eventLoop.exec(); //waiting..
        if(reply->error() == QNetworkReply::NoError){
            //success
            data = reply->readAll();
            if(data.compare("OK") == 0){
                Logger::log("Uploaded successfully!", NORMAL);
                outputFile.rename(output + ".uploaded");
                //outputFile.remove(); only renaming for debugging purposes
            }
            else{
                Logger::log("Upload failed: " + data, NORMAL);
            }
        }
        else{
            //failure
            Logger::log("Upload failed: " + reply->errorString(), NORMAL);
        }
    }
    else{
        //TODO: support for MDXfind needs to be added...
    }
    emit finished();
}

