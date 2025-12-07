#include "MoodTracker.h"
#include "libs/json/json.hpp"   // ✅ FIX nlohmann::json
#include <sqlite3.h>
#include <sstream>

using json = nlohmann::json;
// ✅ Intelligent Mood Weight System
double getMoodWeight(const std::string& mood) {
    if (mood == "Happy")   return 1.0;
    if (mood == "Calm")    return 0.8;
    if (mood == "Neutral")return 0.5;
    if (mood == "Anxious")return -0.6;
    if (mood == "Angry")  return -0.8;
    if (mood == "Sad")    return -1.0;

    return 0.0;
}

// ✅ Convert Mood + Level → Mental Health %
int computeMentalScore(const std::string& mood, int level) {
    double weight = getMoodWeight(mood);

    double raw = (level * weight + 10) * 5;

    if (raw < 0) raw = 0;
    if (raw > 100) raw = 100;

    return static_cast<int>(raw);
}

MoodTracker::MoodTracker(Database& db) : db(db) {}

void MoodTracker::addMood(const std::string& body) {
    json j = json::parse(body);

    std::string mood = j["mood"];
    int level = j["level"].get<int>();
    std::string date = j.value("date", "Today");

    const char* sql = "INSERT INTO moods(mood, level, date) VALUES (?, ?, ?)";
    // ✅ Keep only last 7 mood records
sqlite3_exec(db.getHandle(),
    "DELETE FROM moods WHERE id NOT IN (SELECT id FROM moods ORDER BY id DESC LIMIT 7)",
    nullptr, nullptr, nullptr);


    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db.getHandle(), sql, -1, &stmt, nullptr);

    sqlite3_bind_text(stmt, 1, mood.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 2, level);
    sqlite3_bind_text(stmt, 3, date.c_str(), -1, SQLITE_TRANSIENT);

    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
}

std::string MoodTracker::getAllMoods() {
    json arr = json::array();

    const char* sql = "SELECT mood, level, date FROM moods ORDER BY id DESC";
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db.getHandle(), sql, -1, &stmt, nullptr);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        json obj;
        obj["mood"]  = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        obj["level"] = sqlite3_column_int(stmt, 1);
        obj["date"]  = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        arr.push_back(obj);
    }

    sqlite3_finalize(stmt);
    return arr.dump();
}
std::string MoodTracker::checkCrisisStatus() {
    const char* sql =
        "SELECT mood, level FROM moods ORDER BY id DESC LIMIT 10";

    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db.getHandle(), sql, -1, &stmt, nullptr);

    int negativeCount = 0;
    bool resetByHappy = false;

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        std::string mood =
            reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        int level = sqlite3_column_int(stmt, 1);

        // ✅ RESET RULE: Happy with level > 5 clears crisis
        if (mood == "Happy" && level > 5) {
            resetByHappy = true;
            break;
        }

        // ✅ NEGATIVE MOODS THAT COUNT TOWARDS CRISIS
        if ((mood == "Sad" || mood == "Angry" || mood == "Anxious") && level >= 6) {
            negativeCount++;
        }
    }

    sqlite3_finalize(stmt);

    bool crisis = false;

    // ✅ Crisis only if NOT reset by happy & 2+ negatives
    if (!resetByHappy && negativeCount >= 2) {
        crisis = true;
    }

    json j;
    j["crisis"] = crisis;

    if (crisis) {
        j["message"] =
            "⚠ You have been feeling distressed for multiple days. Please consider reaching out for professional support or a trusted person.";
    } else {
        j["message"] =
            "✅ You are currently out of crisis based on your recent mood trend.";
    }

    return j.dump();
}



std::string MoodTracker::getWeeklyStats() {
    json arr = json::array();

    const char* sql =
        "SELECT mood, level FROM moods ORDER BY id DESC LIMIT 7";

    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db.getHandle(), sql, -1, &stmt, nullptr);

    int idx = 1;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        json obj;

        std::string mood =
            reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));

        int level = sqlite3_column_int(stmt, 1);  // ✅ KEEP 1–10 scale

        obj["label"] = "D" + std::to_string(idx);
        obj["mood"]  = mood;     // ✅ THIS WAS MISSING
        obj["level"] = level;    // ✅ RAW LEVEL 1–10

        arr.push_back(obj);
        idx++;
    }

    sqlite3_finalize(stmt);
    return arr.dump();
}



std::string MoodTracker::getSuggestion() {
    const char* sql = "SELECT AVG(level) FROM moods";
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db.getHandle(), sql, -1, &stmt, nullptr);

    double avg = 0;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        avg = sqlite3_column_double(stmt, 0);
    }
    sqlite3_finalize(stmt);

    std::string msg;

    if (avg >= 8) {
        msg = "🌟 You are emotionally strong! Challenge yourself with new goals today.";
    }
    else if (avg >= 5) {
        msg = "🙂 You are doing okay. Try meditation, walking, or writing your thoughts.";
    }
    else {
        msg = "💙 You seem stressed. Please take rest, reduce screen time, and talk to someone you trust.";
    }

    json j;
    j["message"] = msg;
    j["avg"] = avg;
    return j.dump();
}


std::string MoodTracker::getAverageMoodPercent() {
    const char* sql = "SELECT mood, level FROM moods";
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db.getHandle(), sql, -1, &stmt, nullptr);

    double total = 0;
    int count = 0;

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        std::string mood =
            reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));

        int level = sqlite3_column_int(stmt, 1);

        int score = computeMentalScore(mood, level);
        total += score;
        count++;
    }

    sqlite3_finalize(stmt);

    int avg = (count == 0) ? 0 : static_cast<int>(total / count);

    json j;
    j["percent"] = avg;
    return j.dump();
}

std::string MoodTracker::getFrequentMood() {
    json out;
    out["mood"] = "";

    const char* sql =
        "SELECT mood, COUNT(*) as c FROM moods GROUP BY mood ORDER BY c DESC LIMIT 1";

    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db.getHandle(), sql, -1, &stmt, nullptr);

    if (sqlite3_step(stmt) == SQLITE_ROW)
        out["mood"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));

    sqlite3_finalize(stmt);
    return out.dump();
}

std::string MoodTracker::getLatestMood() {
    json out;
    out["mood"] = "";

    const char* sql = "SELECT mood FROM moods ORDER BY id DESC LIMIT 1";

    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db.getHandle(), sql, -1, &stmt, nullptr);

    if (sqlite3_step(stmt) == SQLITE_ROW)
        out["mood"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));

    sqlite3_finalize(stmt);
    return out.dump();
}
