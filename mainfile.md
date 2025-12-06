# 🧠 Mental Health Support & Mood Tracking System  
**(Offline Secure Web App)**

A privacy-focused, offline-first mental wellness application that helps users track their mood, maintain personal journals, and visualize emotional trends — all without using the internet or cloud storage.

---

## 📌 Table of Contents
- [Problem Statement](#-problem-statement)
- [Solution Overview](#-solution-overview)
- [Key Features](#-key-features)
- [Tech Stack](#-tech-stack)
- [System Architecture](#-system-architecture)
- [Installation & Setup](#-installation--setup)
- [How to Run the Project](#-how-to-run-the-project)
- [Database Schema](#-database-schema)
- [Advantages](#-advantages)
- [Limitations](#-limitations)
- [Future Enhancements](#-future-enhancements)
- [Use Cases](#-use-cases)
- [Conclusion](#-conclusion)

---

## ❓ Problem Statement

In today’s fast-paced lifestyle, many people:

- Struggle to **track their mental health patterns**
- Forget their **daily emotional state**
- Have **no private space** to express feelings
- Fear **data privacy issues** with online mental health apps
- Lack **regular mental health self-assessment**

### ❌ Limitations of Existing Solutions:
- Depend on **internet & cloud storage**
- Pose **privacy and data breach risks**
- Lack **offline accessibility**
- No single-user private ecosystem

---

## ✅ Solution Overview

The **Mental Health Support & Mood Tracking System** is a **completely offline mental wellness web application** that allows users to:

- Log in using a secure PIN
- Track daily moods
- Maintain a personal journal
- View emotional analytics on a dashboard
- Reset forgotten PIN using a security question
- Switch between light and dark themes
- Store all data securely in a **local SQLite database**

This ensures **100% privacy and user control**.

---

## ✨ Key Features

### 🔐 Authentication & Security
- PIN-based Login
- Change PIN from Settings
- Forgot PIN using Security Question
- Fully offline authentication

### 🌈 Mood Tracking
- Add daily mood
- Mood intensity level (1–10)
- Date-wise mood history
- Weekly mood analytics

### 📘 Journal System
- Secure personal journaling
- Add daily thoughts
- View all previous entries
- Stored locally

### 📊 Dashboard Analytics
- Average mood percentage
- Total journal count
- Most frequent mood
- Latest mood
- Weekly mood trend chart
- Daily mental health suggestion

### 🎨 Theme Control
- Light & Dark themes
- Theme controlled only from Settings
- Theme persists after logout

### 💾 Offline Storage
- All data stored locally using SQLite
- No cloud, no internet, no third-party servers

---

## 🛠 Tech Stack

### 🧠 Backend
- C++
- WinSock2 (Custom HTTP Server)
- SQLite3
- nlohmann/json (JSON handling)
- CMake Build System

### 🌐 Frontend
- HTML5
- CSS3 (Glassmorphism UI)
- JavaScript (Fetch API, LocalStorage)

### 🗃 Database
- SQLite3  
  - Tables: `users`, `moods`, `journals`  
  - Views: `average_mood`, `frequent_mood`, `latest_mood`

---

## 🏗 System Architecture


Frontend (HTML, CSS, JS)
⬇ Fetch API
Backend (C++ HTTP Server)
⬇ SQLite3
Local Database Storage (mental_health.db)

✅ Advantages

✅ 100% Offline

✅ Complete Privacy

✅ No Internet Required

✅ Secure Local Storage

✅ Lightweight & Fast

✅ No Subscription Needed

✅ User-Friendly Interface

✅ Personalized Mental Analytics

❌ Limitations

❌ No real-time therapist chat

❌ No cloud backup (manual backup required)

❌ No multi-user support currently

❌ No AI-based prediction yet

🚀 Future Enhancements

AI-based mood prediction

Multi-user profiles

Encrypted cloud backup (optional)

PDF mental health report generation

Emergency contact system

Machine learning trend analysis

👥 Use Cases

Students for emotional self-assessment

People dealing with anxiety or stress

Users who prefer private journaling

Mental wellness tracking in low-internet areas

✅ Conclusion

The Mental Health Support & Mood Tracking System is a secure, offline-first mental wellness platform that allows users to:

Track daily emotions

Maintain personal journals

Analyze emotional trends

Stay emotionally aware

And do all this with complete privacy and control

This project successfully integrates technology with mental wellness in a simple, secure, and accessible way.
