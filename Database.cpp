#include <iostream>
#include "Database.h"
#include "libs/json/json.hpp"
using json = nlohmann::json;
Database::Database() {
    std::string path = "mental_health.db";   // ✅ since DB is inside build folder
    std::cout << "[DB] Trying to open: " << path << "\n";

    if (sqlite3_open(path.c_str(), &db) != SQLITE_OK) {
        std::cout << "❌ DATABASE OPEN FAILED: " << sqlite3_errmsg(db) << "\n";
        db = nullptr;
    } else {
        std::cout << "✅ DATABASE CONNECTED SUCCESSFULLY!\n";
    }
}

Database::~Database() {
    if (db) sqlite3_close(db);
}

sqlite3* Database::getHandle() {
    return db;
}

bool Database::checkPIN(const std::string& pin) {
    const char* sql = "SELECT pin FROM users LIMIT 1";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }

    bool result = false;

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        std::string dbPin =
            reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));

        // ✅ DEBUG PRINT (IMPORTANT)
        std::cout << "DB PIN = [" << dbPin << "]\n";
        std::cout << "INPUT PIN = [" << pin << "]\n";

        result = (dbPin == pin);
    }

    sqlite3_finalize(stmt);
    return result;
}

std::string Database::getEmergencyContact() {
    const char* sql = "SELECT emergency_contact FROM users WHERE id = 1";
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);

    std::string contact = "112";

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        contact = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
    }

    sqlite3_finalize(stmt);

    json j;
    j["contact"] = contact;
    return j.dump();
}

std::string Database::getSecurityQuestion() {
    const char* sql = "SELECT security_question FROM users WHERE id = 1";
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);

    std::string q = "Security question not set";

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        q = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
    }

    sqlite3_finalize(stmt);

    json j;
    j["question"] = q;
    return j.dump();
}

bool Database::verifySecurityAnswer(const std::string& answer) {
    const char* sql = "SELECT security_answer FROM users WHERE id = 1";
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);

    bool ok = false;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        std::string real =
            reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        ok = (real == answer);
    }

    sqlite3_finalize(stmt);
    return ok;
}

bool Database::setPIN(const std::string& pin) {
    const char* sql = "UPDATE users SET pin = ? WHERE id = 1";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }

    sqlite3_bind_text(stmt, 1, pin.c_str(), -1, SQLITE_TRANSIENT);

    bool success = (sqlite3_step(stmt) == SQLITE_DONE);

    sqlite3_finalize(stmt);
    return success;
}

