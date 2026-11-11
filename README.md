# Astracare

### Astronaut Health Monitoring Application

> *"Your health companion, even when Earth is out of reach."*

Astracare is an **offline-first health monitoring app**, written in **C++**, that helps astronauts track their health, recognise potential problems early, and access reliable medical guidance when help from Earth is minutes (or hours) away.

 **Astracare supports astronauts; it does not replace doctors.** The app provides health guidance and alerts, not medical diagnoses.

---

## Table of Contents

1. [The Problem](#-the-problem)
2. [Our Solution](#-our-solution)
3. [Features](#-features)
4. [How It Works](#-how-it-works)
5. [Example Scenario](#-example-scenario)
6. [Tech Stack](#-tech-stack)
7. [Architecture](#-architecture)
8. [Project Structure](#-project-structure)
9. [Getting Started](#-getting-started)
10. [Team and Task Division](#-team-and-task-division)
11. [Hackathon Timeline](#-hackathon-timeline)
12. [Git Workflow and Code Standards](#-git-workflow-and-code-standards)
13. [Roadmap](#-roadmap)
14. [Disclaimer](#-disclaimer)

---

## The Problem

Space exploration comes with serious health risks: radiation exposure, bone and muscle loss, heart problems, vision changes, weakened immunity and psychological stress.

Imagine an astronaut on the way to Mars suddenly experiencing chest pain, dizziness or difficulty breathing. Communication between Mars and Earth can take **4 to 24 minutes one way**, so an astronaut could wait up to **48 minutes** for a message and reply, before medical professionals have even assessed the emergency. With limited medical resources and no immediate access to doctors, astronauts may have to make critical health decisions on their own.

### The Five Hazards of Spaceflight

| Hazard | Impact |
|---|---|
| Space radiation | Long-term health risk |
| Isolation and confinement | Mental health and wellbeing |
| Distance from Earth | Delayed communication and medical help |
| Gravity fields | Bone, muscle and cardiovascular changes |
| Hostile, closed environments | Constant physical and psychological strain |

---

## Our Solution

Astracare is designed to help astronauts:

- **Monitor their health**: track heart rate, blood pressure, sleep and mood.
- **Detect warning signs**: identify unusual readings and potential health risks.
- **Access offline medical guidance**: reliable procedures with no internet connection.
- **Take informed action**: follow appropriate medical procedures and respond to health concerns.

---

## Features

| Feature | Description |
|---|---|
| **Track** | Log and monitor vital signs and wellbeing (heart rate, blood pressure, sleep, mood, symptoms). |
| **Guide** | Browse stored medical guidance and procedures, fully offline. |
| **Alert** | Flag unusual health readings and trends against configurable reference ranges. |
| **Report** | Save health history and generate medical reports to send to Earth when communication is available. |
| **Respond** | Step-by-step emergency support procedures for urgent situations. |

---

## How It Works

1. **Enter health information**: heart rate, blood pressure, sleep, mood, symptoms.
2. **Analyse health indicators**: the app checks measurements and identifies concerning changes.
3. **Provide next steps**: display alerts and approved guidance, all offline.

---

## Example Scenario

An astronaut on a mission to Mars suddenly feels dizzy and tired.

1. Astracare **records** their symptoms and health measurements.
2. The app **identifies** readings that may require attention.
3. Astracare **provides** access to the appropriate medical procedures.
4. A **medical report** can be shared once communication is available.

---

## Tech Stack

| Layer | Technology |
|---|---|
| Language | C++17 (or C++20) |
| Build system | CMake |
| GUI | Qt 6 (Widgets or QML) |
| Charts | Qt Charts |
| Local storage | SQLite (via Qt SQL or `sqlite3` directly) |
| Guidance content | JSON files bundled into the app (Qt resources / nlohmann-json) |
| Report export | `QPdfWriter` (PDF) and JSON/CSV export |
| Testing | Catch2 or GoogleTest, run through CTest |
| Formatting | clang-format |
| CI (optional) | GitHub Actions |

**Why offline-first?** Every core feature (tracking, alerts, guidance, reports) must work with **zero network access**. Data lives in a local SQLite file, guidance is bundled inside the application, and nothing depends on a server.

---

## Architecture

The most important design rule for a team of six: **keep the core logic separate from the UI.** The core library has no Qt dependency, so it can be tested on its own and everyone can work in parallel without stepping on each other.

```
 ┌─────────────────────────────────────────────────────────┐
 │  UI layer (Qt)                                          │
 │  Dashboard · Track · Guide · Alerts · Reports · SOS     │
 └───────────────┬─────────────────────────────────────────┘
                 │ calls
 ┌───────────────▼─────────────────────────────────────────┐
 │  Core library (pure C++, no Qt)                         │
 │  models · alert engine · guidance loader · report model │
 └───────────────┬─────────────────────────────────────────┘
                 │ reads / writes
 ┌───────────────▼─────────────────────────────────────────┐
 │  Storage (SQLite) + bundled guidance (JSON)             │
 └─────────────────────────────────────────────────────────┘
```

### Core data models

```cpp
struct VitalReading {
    int         id;
    std::string timestamp;      // ISO 8601
    int         heartRate;      // bpm
    int         systolic;       // mmHg
    int         diastolic;      // mmHg
    double      sleepHours;
    int         mood;           // 1 to 5
    std::string symptoms;       // free text or comma-separated tags
};

enum class AlertLevel { Normal, Caution, Urgent };

struct Alert {
    AlertLevel  level;
    std::string title;
    std::string message;
    std::string guidanceId;     // links to a guidance/procedure entry
};
```

---

## Project Structure

```
astracare/
├── CMakeLists.txt
├── core/                       # Pure C++ library (no Qt)
│   ├── include/astracare/
│   │   ├── models.h
│   │   ├── alert_engine.h
│   │   ├── guidance.h
│   │   └── report.h
│   └── src/
├── storage/                    # SQLite access layer
│   ├── include/astracare/database.h
│   └── src/
├── ui/                         # Qt application
│   ├── main.cpp
│   ├── MainWindow.{h,cpp}
│   ├── pages/                  # Dashboard, Track, Guide, Alerts, Reports, Emergency
│   └── widgets/                # Reusable components
├── resources/
│   ├── guidance/               # Offline medical guidance (JSON)
│   ├── ranges.json             # Reference ranges for vitals
│   └── styles/                 # Stylesheet / theme
├── tests/                      # Unit tests for core + storage
├── docs/                       # Pitch deck, diagrams, screenshots
└── README.md
```

---

## Getting Started

### Prerequisites

- A C++17-capable compiler (GCC 9+, Clang 10+ or MSVC 2019+)
- CMake 3.20+
- Qt 6 (with the Widgets, Charts and Sql modules)

### Build and run

```bash
git clone https://github.com/<your-org>/astracare.git
cd astracare
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
./build/ui/astracare          # on Windows: build\ui\Release\astracare.exe
```

### Run the tests

```bash
cd build
ctest --output-on-failure
```

### Check offline mode

Disconnect from the network (Wi-Fi off, cable out), then launch the app and confirm that every page, alert, guidance entry and report export still works.

---

## Team and Task Division

| # | Role | Owner | Responsibilities | Main deliverables |
|---|---|---|---|---|
| 1 | **Project Lead and Pitch** | @[Sphiwe](https://github.com/smMazibuko) | Keeps scope under control, manages the board and merges, owns the demo script and pitch deck, writes docs | README, pitch deck, demo script, final demo |
| 2 | **UI Lead (Qt)** | @[Caitlyne](https://github.com/Allison-kk) | Main window, navigation, theme and stylesheet, dashboard, reusable widgets, space-themed look | Main window, Dashboard page, UI kit |
| 3 | **Vitals Tracking (Track)** | @[Lethabo](https://github.com/ST10448170) | Input forms for heart rate, blood pressure, sleep, mood and symptoms; input validation; history table; trend charts | Track page, charts, validation |
| 4 | **Alerts Engineer (Alert)** | @[Sunday](https://github.com/ST10394012) | Reference ranges, analysis logic, warning levels (Normal, Caution, Urgent), trend detection, unit tests | `alert_engine`, `ranges.json`, tests, alert banner widget |
| 5 | **Medical Content and Emergency (Guide and Respond)** | @[Zandile](https://github.com/Nkateko-Zandi) | Research and write offline guidance from reputable sources, build step-by-step emergency procedures, searchable guidance library | Guidance JSON, Guide page, Emergency page, source list |
| 6 | **Storage, Build and Reports (Report)** | @[Madiki](https://github.com/Caleb1128) | CMake setup, SQLite schema and data layer, report generation and export (PDF/JSON), CI, offline QA | `database`, Report page, export, CMake and CI, QA checklist |

### How the pieces connect

- **Role 6 → everyone**: provides the CMake skeleton and the database API in the first phase. *Everyone depends on this, so it ships first.*
- **Role 3 → Role 6**: vitals are saved through the `Database` class.
- **Role 3 → Role 4**: the alert engine takes `VitalReading` data and returns `Alert` objects.
- **Role 4 → Role 2**: alert levels drive the colours and banners on the dashboard.
- **Role 4 → Role 5**: each alert carries a `guidanceId` that opens the matching guidance or emergency procedure.
- **Role 5 → Role 6**: reports can include the relevant guidance that was accessed.

### Shared responsibilities

- Everyone reviews at least one pull request per day.
- Everyone tests offline mode before their feature is marked done.
- Everyone contributes to the demo rehearsal.

---

## Hackathon Timeline

| Phase | Goal | Who |
|---|---|---|
| **1. Setup** | Everyone builds the empty app, agree on models and interfaces (headers), CMake skeleton, task board | Everyone (Roles 1 and 6 drive) |
| **2. Build core** | Track page, alert engine, guidance content, database layer, main window | Roles 2 to 6 in parallel |
| **3. Integrate** | Connect alerts to guidance, dashboard to live data, reports | Everyone |
| **4. Polish and test** | Offline testing, bug fixes, UI polish, sample data for the demo | Everyone |
| **5. Present** | Pitch deck final, demo rehearsal (run it at least twice) | Lead and everyone |

---

## Git Workflow and Code Standards

- `main` is always buildable and demo-ready. Never push directly to it.
- Create a branch per task: `feature/<role>-<short-name>`, for example `feature/alerts-bp-ranges`.
- Open a pull request and get **one approval** before merging.
- Use clear commits, such as `feat: add heart rate input form` or `fix: sleep chart crash`.
- Keep pull requests small and merge often to avoid conflicts.
- Never commit `build/` folders or IDE files. Add them to `.gitignore`.
- Run `clang-format` before committing.
- Header changes (`models.h` and the like) need a heads-up in the team chat, since everyone's code depends on them.

---

## Roadmap

- [ ] Integration with wearable sensors
- [ ] Voice-controlled input for hands-busy situations
- [ ] Multi-crew profiles and shared mission health logs
- [ ] Mental wellbeing check-ins and guided exercises
- [ ] Sync and report transmission queue for when communication returns
- [ ] Review of guidance content by qualified medical professionals

---

## Disclaimer

Astracare is a hackathon prototype for educational and demonstration purposes. It provides health guidance and alerts, **not medical diagnoses**, and is not a certified medical device. Reference ranges and guidance content should be reviewed by qualified medical professionals before any real-world use.

---

## Team

| Name | Role | GitHub |
|---|---|---|
| Sphiwe | Project Lead and Pitch | [@smMazibuko](https://github.com/smMazibuko) |
| Caitlyne | UI Lead (Qt) | [@Allison-kk](https://github.com/Allison-kk) |
| Lethabo | Vitals Tracking | [@ST10448170](https://github.com/ST10448170) |
| Sunday | Alerts Engineer | [@ST10394012](https://github.com/ST10394012) |
| Zandile | Medical Content and Emergency | [@Nkateko-Zandi](https://github.com/Nkateko-Zandi) |
| Madiki | Storage, Build and Reports | [@Caleb1128](https://github.com/Caleb1128) |

---

<p align="center"><i>"No matter the distance, health comes first."</i></p>
