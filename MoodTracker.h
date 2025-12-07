#ifndef MOODTRACKER_H
#define MOODTRACKER_H
#include<string>
#include "Database.h"

class MoodTracker {
private:
    Database &db;
    
public:
    MoodTracker(Database &d);


    void        addMood(const std::string& body);
    std::string getAllMoods();

    // Dashboard stats:
    std::string getWeeklyStats();
    std::string getSuggestion();
    std::string getAverageMoodPercent();  // { "percent": 72 }
    std::string getFrequentMood();        // { "mood": "Happy" }
    std::string getLatestMood();          // { "mood": "Sad" }
    std::string checkCrisisStatus();
};

#endif
