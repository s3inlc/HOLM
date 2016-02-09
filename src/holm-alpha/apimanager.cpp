#include "apimanager.h"

ApiManager::ApiManager(QObject *parent) : QObject(parent){
    isValid = false;
    Logger::log("Check for available API key...", DEBUG);
    QFile f(DATA + QString("/") + API_KEY_PATH);
    if(!f.open(QFile::ReadOnly | QFile::Text)){
        Logger::log("Failed to open an API key file!", INCREASED);
        return;
    }
    QTextStream in(&f);
    apiKey = in.readAll();
    isValid = true;
}

bool ApiManager::validApiAvailable(){
    return isValid;
}

QString ApiManager::getKey(){
    if(!isValid){
        return "";
    }
    return apiKey;
}

void ApiManager::setKey(QString k){
    QNetworkAccessManager manager(this);
    QEventLoop eventLoop;
    connect(&manager, SIGNAL(finished(QNetworkReply*)), &eventLoop, SLOT(quit()));
    QUrl url(QString(API) + "?holm=check&key=" + k);
    Logger::log("Check API key for validity!", INCREASED);
    Logger::log("CALL: " + url.toString(), DEBUG);
    QNetworkReply *reply = manager.get(QNetworkRequest(url));
    eventLoop.exec(); //waiting..
    QString data;
    if(reply->error() == QNetworkReply::NoError){
        //success
        data = reply->readAll();
        if(data.compare("CONFIRM") == 0){
            Logger::log("API key is valid!", DEBUG);
            isValid = true;
        }
        else{
            Logger::log("API key is not valid: " + data, NORMAL);
            isValid = false;
        }
    }
    else{
        //failure
        Logger::log("API request failed with: " + reply->errorString(), NORMAL);
        isValid = false;
    }
    if(isValid){
        QFile file(DATA + QString("/") + API_KEY_PATH);
        if(file.open(QIODevice::ReadOnly)){
            QTextStream stream(&file);
            stream << k;
        }
        file.close();
        apiKey = k;
    }
    delete reply;
}

