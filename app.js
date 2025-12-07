// ✅ GLOBAL ROUTE PROTECTION
(function protectPages() {
    const isLoggedIn = localStorage.getItem("loggedIn") === "true";
    const currentPage = window.location.pathname.toLowerCase();

    // ✅ Pages that DO NOT need login
    const publicPages = ["index.html", "intro.html"];

    const isPublic = publicPages.some(p => currentPage.includes(p));

    if (!isLoggedIn && !isPublic) {
        alert("⚠ Please login first!");
        window.location.href = "index.html";
    }
})();

const API_URL = "http://localhost:5000";

/* ---------------------------------------------
   POPUP
--------------------------------------------- */
function showPopup(msg) {
    const popup = document.getElementById("popup");
    if (!popup) return;
    popup.innerText = msg;
    popup.classList.add("show");
    setTimeout(() => popup.classList.remove("show"), 2000);
}

/* ---------------------------------------------
   CRISIS & EMERGENCY
--------------------------------------------- */
function resetMood() {
    if (!confirm("Delete all mood history?")) return;

    fetch(`${API_URL}/mood/reset`, { method: "POST" })
        .then(r => r.json())
        .then(d => {
            showPopup("Mood history cleared ✅");
            loadMoods();
        });
}
function resetJournal() {
    if (!confirm("Delete all journal entries?")) return;

    fetch(`${API_URL}/journal/reset`, { method: "POST" })
        .then(r => r.json())
        .then(d => {
            showPopup("Journal cleared ✅");
            loadJournal();
        });
}

// Crisis detection: uses intelligent scoring from backend
function checkCrisisStatus() {
    fetch(`${API_URL}/stats/crisis`)
        .then(r => r.json())
        .then(d => {
            const box = document.getElementById("emergencyBox");
            const el = document.getElementById("suggestionText");

            if (d.crisis) {
                if (el) el.innerText = d.message;
                if (box) box.style.display = "block";
                showPopup("⚠ Crisis detected. Emergency help enabled.");
            } else {
                if (box) box.style.display = "none";
            }
        });
}


// Emergency call helper (if you wire emergency/contact on backend)
function callEmergency() {
    fetch(`${API_URL}/emergency/contact`)
        .then(r => r.json())
        .then(d => {
            const number = d.contact;
            if (number) {
                window.location.href = `tel:${number}`;
            } else {
                showPopup("No emergency contact configured.");
            }
        })
        .catch(() => {
            showPopup("Unable to fetch emergency contact.");
        });
}

/* ---------------------------------------------
   THEME (LIGHT / DARK)
--------------------------------------------- */
function toggleThemeFromSettings() {
    const isDark = document.body.classList.toggle("dark");
    localStorage.setItem("theme", isDark ? "dark" : "light");
    showPopup(isDark ? "Dark Mode Enabled 🌙" : "Light Mode Enabled ☀️");
}

function restoreTheme() {
    const theme = localStorage.getItem("theme");
    if (theme === "dark") {
        document.body.classList.add("dark");
    } else {
        document.body.classList.remove("dark");
    }
}

/* ---------------------------------------------
   SIMPLE SESSION
--------------------------------------------- */
function setLoggedIn(v) {
    localStorage.setItem("loggedIn", v ? "true" : "false");
}

function isLoggedIn() {
    return localStorage.getItem("loggedIn") === "true";
}

function requireAuth() {
    const loggedIn = localStorage.getItem("loggedIn");
    console.log("AUTH CHECK:", loggedIn);

    if (loggedIn !== "true") {
        showPopup("Please login first");
        setTimeout(() => {
            window.location.href = "index.html";
        }, 500);
    }
}

function logout() {
    localStorage.removeItem("loggedIn");
    window.location.href = "index.html";
}

/* ---------------------------------------------
   FORGOT PIN FLOW
--------------------------------------------- */
function showForgot() {
    const box = document.getElementById("forgotBox");
    if (box) box.style.display = "block";

    fetch(`${API_URL}/auth/question`)
        .then(r => r.json())
        .then(d => {
            const q = document.getElementById("secQuestion");
            if (q) q.innerText = d.question;
        })
        .catch(() => showPopup("Unable to load security question."));
}

function verifyAnswer() {
    const answerEl = document.getElementById("secAnswer");
    if (!answerEl) return;
    const answer = answerEl.value;

    fetch(`${API_URL}/auth/verify`, {
        method: "POST",
        headers: {"Content-Type": "application/json"},
        body: JSON.stringify({ answer })
    })
    .then(r => r.json())
    .then(d => {
        if (d.verified) {
            const box = document.getElementById("resetBox");
            if (box) box.style.display = "block";
            showPopup("Verified ✅");
        } else {
            showPopup("Wrong Answer ❌");
        }
    })
    .catch(() => showPopup("Verification failed."));
}

function resetPin() {
    const newPinEl = document.getElementById("newPinReset");
    if (!newPinEl) return;
    const newPin = newPinEl.value;

    if (!newPin) {
        showPopup("Enter a new PIN.");
        return;
    }

    fetch(`${API_URL}/auth/reset`, {
        method: "POST",
        headers: {"Content-Type": "application/json"},
        body: JSON.stringify({ newPin })
    })
    .then(r => r.json())
    .then(d => {
        if (d.success) {
            showPopup("PIN Reset Successfully ✅");
            setTimeout(() => location.reload(), 1200);
        } else {
            showPopup("Reset Failed ❌");
        }
    })
    .catch(() => showPopup("Reset failed ❌"));
}

/* ---------------------------------------------
   LOGIN (POST /login)
--------------------------------------------- */
function login() {
    console.log("LOGIN START");

    const pinEl = document.getElementById("pin");
    if (!pinEl) return;
    const pin = pinEl.value;
    console.log("PIN:", pin);

    if (!pin) {
        showPopup("Enter your PIN");
        return;
    }

    fetch(`${API_URL}/login`, {
        method: "POST",
        headers: {
            "Content-Type": "application/json"
        },
        body: JSON.stringify({ pin })
    })
    .then(res => {
        console.log("STATUS:", res.status);
        return res.text();
    })
    .then(data => {
        console.log("RAW RESPONSE:", data);
        const json = JSON.parse(data);

        if (json.success) {
            localStorage.setItem("loggedIn", "true");
            window.location.href = "dashboard.html";
        } else {
            alert("INVALID PIN ❌");
        }
    })
    .catch(err => {
        console.error("FETCH ERROR:", err);
        alert("BACKEND NOT REACHABLE ❌");
    });
}

/* ---------------------------------------------
   PIN REGISTRATION  (POST /register)
--------------------------------------------- */
function registerPin() {
    const pinEl = document.getElementById("newPin");
    if (!pinEl) return;
    const pin = pinEl.value;

    if (!pin) return showPopup("Enter new PIN");

    fetch(`${API_URL}/register`, {
        method: "POST",
        headers: {"Content-Type":"application/json"},
        body: JSON.stringify({ pin })
    })
    .then(r=>r.json())
    .then(d=>{
        if (d.success) showPopup("PIN updated successfully!");
        else showPopup("Error updating PIN");
    })
    .catch(() => showPopup("Backend error"));
}

/* ---------------------------------------------
   ADD MOOD  (POST /mood/add)
--------------------------------------------- */
function addMood() {
    const moodEl = document.getElementById("moodSelect");
    const levelEl = document.getElementById("level");
    if (!moodEl || !levelEl) return;

    const mood = moodEl.value;
    const level = Number(levelEl.value);

    if (!level || level < 1 || level > 10)
        return showPopup("Level must be 1–10");

    fetch(`${API_URL}/mood/add`, {
        method:"POST",
        headers:{"Content-Type":"application/json"},
        body:JSON.stringify({ mood, level })
    })
    .then(r=>r.json())
    .then(d=>{
        showPopup(d.message);
        loadMoods();
    })
    .catch(() => showPopup("Error saving mood"));
}

/* ---------------------------------------------
   LOAD MOODS  (GET /mood/all)
--------------------------------------------- */
function loadMoods() {
    fetch(`${API_URL}/mood/all`)
    .then(r => r.json())
    .then(rows => {
        const list = document.getElementById("moodList");
        if (!list) return;
        list.innerHTML = "";

        if (!rows.length) {
            list.innerHTML = "<p>No moods yet.</p>";
            return;
        }

        rows.forEach(r => {
            const li = document.createElement("li");
            li.innerHTML = `
                <strong>${r.date}</strong><br>
                Mood: ${r.mood}<br>
                Level: ${r.level}
            `;
            list.appendChild(li);
        });
    })
    .catch(() => showPopup("Error loading moods"));
}

/* ---------------------------------------------
   SAVE JOURNAL  (POST /journal/add)
--------------------------------------------- */
function saveJournal() {
    const entryEl = document.getElementById("entry");
    if (!entryEl) return;

    const text = entryEl.value.trim();
    if (!text) return showPopup("Write something!");

    fetch(`${API_URL}/journal/add`, {
        method:"POST",
        headers:{"Content-Type":"application/json"},
        body:JSON.stringify({ text })
    })
    .then(r=>r.json())
    .then(d=>{
        showPopup(d.message);
        entryEl.value = "";
        loadJournal();
    })
    .catch(() => showPopup("Error saving journal"));
}

/* ---------------------------------------------
   LOAD JOURNAL  (GET /journal/all)
--------------------------------------------- */
function loadJournal() {
    fetch(`${API_URL}/journal/all`)
    .then(r=>r.json())
    .then(rows=>{
        const list = document.getElementById("journalList");
        if (!list) return;
        list.innerHTML = "";

        if (!rows.length) {
            list.innerHTML = "<p>No entries yet.</p>";
            return;
        }

        rows.forEach(x=>{
            const li = document.createElement("li");
            li.innerHTML = `
                <strong>${x.date}</strong><br>
                ${x.text}
            `;
            list.appendChild(li);
        });
    })
    .catch(() => showPopup("Error loading journal"));
}

/* ---------------------------------------------
    DASHBOARD (avg, count, frequent, latest, crisis)
--------------------------------------------- */
function loadDashboard() {
    // require login for dashboard
    requireAuth();

    // Average mood %
    fetch(`${API_URL}/stats/averageMood`)
    .then(r=>r.json())
    .then(d=>{
        const percent = d.percent || 0;
        const txt = document.getElementById("avgMoodVal");
        const circle = document.querySelector("circle.progress");
        if (txt) txt.innerText = percent + "%";
        if (circle) circle.style.strokeDashoffset = 314 - (314 * percent / 100);
    });

    // Journal count
    fetch(`${API_URL}/journal/count`)
    .then(r=>r.json())
    .then(d=>{
        const el = document.getElementById("journalCount");
        if (el) el.innerText = d.count;
    });

    // Frequent mood
    fetch(`${API_URL}/stats/frequentMood`)
    .then(r=>r.json())
    .then(d=>{
        const el = document.getElementById("freqMood");
        if (el) el.innerText = d.mood || "-";
    });

    // Latest mood
// ✅ Weekly chart WITH COLORS
fetch(`${API_URL}/stats/weeklyMood`)
.then(r => r.json())
.then(data => {
    const container = document.getElementById("weeklyChart");
    if (!container) {
        console.log("weeklyChart div not found");
        return;
    }

    container.innerHTML = "";

    const moodColors = {
        "Happy": "#74cb98ff",
        "Sad": "#bd4141ff",
        "Angry": "#712e2eff",
        "Anxious": "#ffba61ff",
        "Neutral": "#6b93ffff"
    };

    console.log("Weekly data:", data);

    data.forEach(d => {
        const bar = document.createElement("div");
        bar.className = "bar";

        const barInner = document.createElement("div");
        barInner.className = "bar-inner";

        // ✅ FORCE HEIGHT AND COLOR
        barInner.style.height = (d.level * 10) + "%";
        barInner.style.backgroundColor = moodColors[d.mood] || "black";

        const value = document.createElement("div");
        value.className = "bar-value";
        value.innerText = d.mood + " - " + d.level;

        const label = document.createElement("div");
        label.className = "bar-label";
        label.innerText = d.label;

        bar.appendChild(barInner);
        bar.appendChild(value);
        bar.appendChild(label);

        container.appendChild(bar);
    });
})
.catch(err => {
    console.log("Weekly chart error:", err);
});



    // Suggestion
    fetch(`${API_URL}/suggestion/today`)
    .then(r=>r.json())
    .then(d=>{
        const el = document.getElementById("suggestionText");
        if (el) el.innerText = d.message || "Keep going, one day at a time.";
    })
    .catch(() => {
        const el = document.getElementById("suggestionText");
        if (el) el.innerText = "Unable to load suggestion.";
    });
    checkCrisisStatus();
}
