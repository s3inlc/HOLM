#ifndef DEFINES
#define DEFINES

#include <QString>

#define API "https://hashes.org/api.php"

enum RunType {
    NO_TYPE,
    GENERATE,
    SINGLE_TASK,
    MULTI_TASK
};

enum BinaryType {
    OCLHASHCAT,
    HASHCAT,
    MDXFIND
};

struct Configuration {
    BinaryType application;
    QString list;
    QString call;
    int algorithm;
    bool newList;
};

#endif // DEFINES

