cmake_minimum_required(VERSION 3.10)
project(mental_health_backend)

set(CMAKE_CXX_STANDARD 17)

include_directories(.)
include_directories(../database)
include_directories(libs/json)

add_executable(backend
    main.cpp
    ApiServer.cpp
    Database.cpp
    MoodTracker.cpp
    JournalManager.cpp
    Encryption.cpp
)


target_link_libraries(backend sqlite3 ws2_32)
