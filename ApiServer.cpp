#include "ApiServer.h"
#include "Database.h"
#include "MoodTracker.h"
#include "JournalManager.h"

#include <iostream>
#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>

#include "libs/json/json.hpp"
using json = nlohmann::json;

#pragma comment(lib, "ws2_32.lib")

Database db;
MoodTracker mood(db);
JournalManager journal(db);

// ----------------- Helpers -----------------

// Extract JSON body from full HTTP request
std::string extractBody(const std::string &req) {
    size_t headerEnd = req.find("\r\n\r\n");
    if (headerEnd == std::string::npos) return "";

    std::string body = req.substr(headerEnd + 4);

    // Trim trailing whitespace / CRLF
    while (!body.empty() &&
           (body.back() == '\n' || body.back() == '\r' || body.back() == ' '))
        body.pop_back();

    return body;
}

// Send JSON response with CORS
void sendResponse(SOCKET client, const std::string &body) {
    std::string response =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: application/json\r\n"
        "Access-Control-Allow-Origin: *\r\n"
        "Access-Control-Allow-Headers: Content-Type\r\n"
        "Access-Control-Allow-Methods: POST, GET, OPTIONS\r\n"
        "Connection: close\r\n"
        "Content-Length: " + std::to_string(body.size()) + "\r\n\r\n" +
        body;

    send(client, response.c_str(), (int)response.size(), 0);
}

// CORS preflight
void sendOptions(SOCKET client) {
    std::string response =
        "HTTP/1.1 204 No Content\r\n"
        "Access-Control-Allow-Origin: *\r\n"
        "Access-Control-Allow-Headers: Content-Type\r\n"
        "Access-Control-Allow-Methods: POST, GET, OPTIONS\r\n"
        "Connection: close\r\n\r\n";

    send(client, response.c_str(), (int)response.size(), 0);
}

// ----------------- Server loop -----------------

void ApiServer::start(int port) {
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);

    SOCKET server_fd = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port);

    bind(server_fd, (sockaddr*)&serverAddr, sizeof(serverAddr));
    listen(server_fd, 10);

    std::cout << "\n✅ Backend running at http://localhost:" << port << "\n";

    while (true) {
        SOCKET client = accept(server_fd, nullptr, nullptr);
        if (client == INVALID_SOCKET) continue;

        // ---------- read first chunk ----------
        char buffer[4096];
        int bytes = recv(client, buffer, sizeof(buffer), 0);
        if (bytes <= 0) {
            closesocket(client);
            continue;
        }

        std::string req(buffer, bytes);

        // ---------- if POST, read remaining body using Content-Length ----------
        size_t lengthPos = req.find("Content-Length:");
        if (lengthPos != std::string::npos) {
            size_t lineEnd = req.find("\r\n", lengthPos);
            std::string lenStr = req.substr(lengthPos + 15,
                                            lineEnd - (lengthPos + 15));
            int bodyLen = std::stoi(lenStr);

            size_t bodyStart = req.find("\r\n\r\n");
            if (bodyStart != std::string::npos) {
                size_t alreadyHave = req.size() - (bodyStart + 4);

                while (alreadyHave < (size_t)bodyLen) {
                    bytes = recv(client, buffer, sizeof(buffer), 0);
                    if (bytes <= 0) break;
                    req.append(buffer, bytes);
                    alreadyHave += bytes;
                }
            }
        }

        std::cout << "\n📥 REQUEST RECEIVED:\n" << req << "\n";

        // ---------- ROUTING ----------

        // CORS preflight
        if (req.rfind("OPTIONS", 0) == 0) {
            sendOptions(client);
            closesocket(client);
            continue;
        }

        // LOGIN
        if (req.find("POST /login") != std::string::npos) {
            std::string body = extractBody(req);
            std::cout << "LOGIN BODY: [" << body << "]\n";

            if (body.empty()) {
                sendResponse(client, "{\"success\":false}");
                closesocket(client);
                continue;
            }

            json j = json::parse(body);
            std::string pin = j["pin"];

            bool ok = db.checkPIN(pin);
            sendResponse(client, ok ? "{\"success\":true}" : "{\"success\":false}");
        }

        // REGISTER / change PIN
        else if (req.find("POST /register") != std::string::npos) {
            std::string body = extractBody(req);
            std::cout << "REGISTER BODY: [" << body << "]\n";

            if (body.empty()) {
                sendResponse(client, "{\"success\":false}");
                closesocket(client);
                continue;
            }

            json j = json::parse(body);
            std::string pin = j["pin"];

            bool ok = db.setPIN(pin);
            sendResponse(client, ok ? "{\"success\":true}" : "{\"success\":false}");
        }

        // ADD MOOD
        else if (req.find("POST /mood/add") != std::string::npos) {
            std::string body = extractBody(req);
            mood.addMood(body);
            sendResponse(client, "{\"message\":\"Mood saved\"}");
        }

        // GET ALL MOODS
        else if (req.find("GET /mood/all") != std::string::npos) {
            sendResponse(client, mood.getAllMoods());
        }

        // ADD JOURNAL
        else if (req.find("POST /journal/add") != std::string::npos) {
            std::string body = extractBody(req);
            journal.addEntry(body);
            sendResponse(client, "{\"message\":\"Journal saved\"}");
        }

        // GET ALL JOURNAL
        else if (req.find("GET /journal/all") != std::string::npos) {
            sendResponse(client, journal.getAll());
        }

        // JOURNAL COUNT
        else if (req.find("GET /journal/count") != std::string::npos) {
            sendResponse(client, journal.getCountJSON());
        }

        // DASHBOARD STATS
        else if (req.find("GET /stats/averageMood") != std::string::npos) {
            sendResponse(client, mood.getAverageMoodPercent());
        } else if (req.find("GET /stats/frequentMood") != std::string::npos) {
            sendResponse(client, mood.getFrequentMood());
        } else if (req.find("GET /stats/latestMood") != std::string::npos) {
            sendResponse(client, mood.getLatestMood());
        } else if (req.find("GET /stats/weeklyMood") != std::string::npos) {
            sendResponse(client, mood.getWeeklyStats());
        }

        // Suggestion
        else if (req.find("GET /suggestion/today") != std::string::npos) {
            sendResponse(client, mood.getSuggestion());
        }
        else if (req.find("GET /auth/question") != std::string::npos) {
    sendResponse(client, db.getSecurityQuestion());
}
        else if (req.find("POST /auth/verify") != std::string::npos) {
    std::string body = extractBody(req);
    auto j = json::parse(body);

    std::string answer = j["answer"];
    bool ok = db.verifySecurityAnswer(answer);

    sendResponse(client, ok ? "{\"verified\":true}" : "{\"verified\":false}");
}
else if (req.find("GET /stats/crisis") != std::string::npos) {
    sendResponse(client, mood.checkCrisisStatus());
}
else if (req.find("POST /mood/reset") != std::string::npos) {
    sqlite3_exec(db.getHandle(), "DELETE FROM moods", nullptr, nullptr, nullptr);
    sendResponse(client, "{\"success\":true}");
}

else if (req.find("POST /journal/reset") != std::string::npos) {
    sqlite3_exec(db.getHandle(), "DELETE FROM journals", nullptr, nullptr, nullptr);
    sendResponse(client, "{\"success\":true}");
}

else if (req.find("GET /emergency/contact") != std::string::npos) {
    sendResponse(client, db.getEmergencyContact());
}

else if (req.find("POST /auth/reset") != std::string::npos) {
    std::string body = extractBody(req);
    auto j = json::parse(body);

    std::string newPin = j["newPin"];
    bool ok = db.setPIN(newPin);

    sendResponse(client, ok ? "{\"success\":true}" : "{\"success\":false}");
}

        // SAFE DEFAULT
        else {
            sendResponse(client, "{\"status\":\"unknown route\"}");
        }

        closesocket(client);
    }

    WSACleanup();
}
