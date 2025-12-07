#ifndef JOURNALMANAGER_H
#define JOURNALMANAGER_H

#include <string>
#include "Database.h"

class JournalManager {
private:
    Database& db;
    std::string key = "secret";   // your encryption key

public:
    JournalManager(Database& db);

    void addEntry(const std::string& body);
    std::string getAll();
    std::string getCountJSON();
};

#endif
