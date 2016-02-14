/*
* Project created by QtCreator 2016-01-28T16:04:57
* Copyright 2016 by Sein Coray
*/

#ifndef DEFINES
#define DEFINES

#include <QString>

#define API "https://hashes.org/api.php"
#define DL_PATH "https://hashes.org/download.php"
#define HOLM_VERSION "1.1"

#define TASKS "tasks"
#define DATA "data"

#define API_KEY_PATH "api.key"

#ifdef WIN32
#include <stdint.h>
#define UINT8 uint8_t
#else
#include <stdint.h>
#define UINT8 uint8_t
#endif

enum BinaryType {
    EMPTY,
    OCLHASHCAT,
    MDXFIND
};

struct DataSet {
    bool isNew;
    QString salt;
};

struct Configuration {
    BinaryType application;
    QString binaryPath;
    QString list;
    QString call;
    QString algorithm;
    bool newList;
};

bool isValidList(QString name);

#endif // DEFINES

