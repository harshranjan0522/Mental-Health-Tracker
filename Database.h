#ifndef DATABASE_H
#define DATABASE_H

#include <string>
#include <sqlite3.h>   // ✅ THIS WAS MISSING

class Database {
private:
    sqlite3* db;       // ✅ THIS WAS MISSING

public:
    Database();
    ~Database();

    sqlite3* getHandle();

    bool checkPIN(const std::string& pin);
    bool setPIN(const std::string& pin);

    std::string getSecurityQuestion();
    std::string getEmergencyContact();
    bool verifySecurityAnswer(const std::string& answer);
};

#endif
