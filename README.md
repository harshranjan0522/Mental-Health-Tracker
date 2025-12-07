# 🧠 Mental Health Support & Mood Tracking System

An offline-first Mental Health Support & Mood Tracking System built with a **C++ backend**, **SQLite database**, and a **HTML/CSS/JavaScript frontend**.

---

## ✅ Prerequisites

Make sure the following are installed on your system before proceeding:

- ✅ **VS Code**
- ✅ **G++ Compiler**
- ✅ **CMake**
- ✅ **SQLite3**

---

## 📁 Project Directory Structure

Arrange your files exactly as shown below:

MENTAL-HEALTH-SUPPORT-SYSTEM/
│
├── backend/
│ ├── libs/json/json.hpp
│ ├── ApiServer.cpp
│ ├── ApiServer.h
│ ├── CMakeLists.txt
│ ├── Database.cpp
│ ├── Database.h
│ ├── Encryption.cpp
│ ├── Encryption.h
│ ├── JournalManager.cpp
│ ├── JournalManager.h
│ ├── main.cpp
│ ├── MoodTracker.cpp
│ ├── MoodTracker.h
│
├── database/
│ ├── mental_health.db
│ ├── schema.sql
│
└── frontend/
  ├── app.js
  ├── dashboard.html
  ├── index.html
  ├── intro.html
  ├── journal.html
  ├── mood.html
  ├── settings.html
  └── style.css


---

# 🚀 Steps to Run the Project

---

## 1️⃣ Create and Configure `backend/build`

1. Open **VS Code**
2. Open the **project root folder**
3. Open the terminal (`View → Terminal`)
4. Run the following commands:

```bash
cd backend
mkdir -p build
cd build

2️⃣ Build (Compile) the Backend

You must still be inside:

backend/build


Now run:

cmake --build .


✅ After successful compilation, an executable file (e.g., backend.exe) will be created inside:

backend/build/


or inside a subfolder like:

backend/build/Debug/

3️⃣ Execute the SQL and Prepare the Database

Now set up the database using schema.sql:

cd database
sqlite3 mental_health.db
.read schema.sql
.exit


✅ This creates all required tables.

✅ Important Step:

Now copy mental_health.db and paste it inside:

backend/build/

4️⃣ Run the Backend Server

Now start the backend:

cd backend/build
./backend.exe


✅ After successful startup, you will see this message:

Database connected successfully - backend is running on local port 5000


⚠️ DO NOT close this terminal. Keep the backend running.

5️⃣ Run the Frontend

Go to:

frontend/


Open intro.html

Start Live Server

✅ It will open in your browser on:

http://localhost:5500


✅ Your frontend is now connected to the backend running on:

http://localhost:5000

🎉 Project is Now Fully Running!

✅ Backend → Port 5000

✅ Frontend → Port 5500

✅ SQLite DB → Connected Successfully

✅ Mood Tracking, Journaling, and Dashboard → Ready

📩 For Any Doubts or Support

Author: Harsh Ranjan
📧 Email: harshranjan7117@gmail.com
