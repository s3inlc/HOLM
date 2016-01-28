#ifndef DEFINES
#define DEFINES

#include <QString>

#define API "https://hashes.org/api.php"
#define HOLM_VERSION "1.0"

#define TASKS "tasks"
#define DATA "data"

#define API_KEY_PATH "api.key"

enum RunType {
    NO_TYPE,
    GENERATE,
    SINGLE_TASK,
    MULTI_TASK
};

enum BinaryType {
    EMPTY,
    OCLHASHCAT,
    HASHCAT,
    MDXFIND
};

struct Configuration {
    BinaryType application;
    QString binaryPath;
    QString list;
    QString call;
    int algorithm;
    bool newList;
};

#endif // DEFINES

