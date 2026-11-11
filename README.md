#  Astracare

### Astronaut Health Monitoring Application

> *"Your health companion, even when Earth is out of reach."*

Astracare is an **offline-first health monitoring app** that helps astronauts track their health, recognise potential problems early, and access reliable medical guidance when help from Earth is minutes (or hours) away.

> ⚠️ **Astracare supports astronauts; it does not replace doctors.** The app provides health guidance and alerts, not medical diagnoses.

---

##  Table of Contents

1. [The Problem](#-the-problem)
2. [Our Solution](#-our-solution)
3. [Features](#-features)
4. [How It Works](#-how-it-works)
5. [Example Scenario](#-example-scenario)
6. [Tech Stack](#-tech-stack)
7. [Project Structure](#-project-structure)
8. [Getting Started](#-getting-started)
9. [Team and Task Division](#-team-and-task-division)
10. [Hackathon Timeline](#-hackathon-timeline)
11. [Git Workflow](#-git-workflow)
12. [Roadmap](#-roadmap)
13. [Disclaimer](#-disclaimer)

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
| 📈 **Track** | Log and monitor vital signs and wellbeing (heart rate, blood pressure, sleep, mood, symptoms). |
| 📚 **Guide** | Browse stored medical guidance and procedures, fully offline. |
| 🚨 **Alert** | Flag unusual health readings and trends against configurable reference ranges. |
| 📝 **Report** | Save health history and generate medical reports to send to Earth when communication is available. |
| 🆘 **Respond** | Step-by-step emergency support procedures for urgent situations. |

---

## How It Works

1. **Enter health information**: heart rate, blood pressure, sleep, mood, symptoms.
2. **Analyse health indicators**: the app checks measurements and identifies concerning changes.
3. **Provide next steps**: display alerts and approved guidance, all offline.

```
 ┌──────────────┐     ┌───────────────────┐     ┌────────────────────────┐
 │  Health data │ ──▶ │  Analysis / Alert │ ──▶ │  Guidance + Procedures │
 │  (input)     │     │  engine           │     │  (offline library)     │
 └──────┬───────┘     └─────────┬─────────┘     └───────────┬────────────┘
        │                       │                           │
        └───────────────┬───────┴───────────────────────────┘
                        ▼
              Local storage (offline)
                        │
                        ▼
        Medical report, shared when comms are available
```

---

## Example Scenario

An astronaut on a mission to Mars suddenly feels dizzy and tired.

1. Astracare **records** their symptoms and health measurements.
2. The app **identifies** readings that may require attention.
3. Astracare **provides** access to the appropriate medical procedures.
4. A **medical report** can be shared once communication is available.

---

## Tech Stack

> *Suggested stack. Update this section to match what the team actually builds.*

| Layer | Technology |
|---|---|
| Frontend | React + TypeScript (Vite) |
| Styling | Tailwind CSS |
| Charts | Recharts |
| Offline support | Progressive Web App (service worker + manifest) |
| Local storage | IndexedDB via Dexie.js |
| Report export | jsPDF (PDF) / JSON export |
| Testing | Vitest + React Testing Library |
| Hosting (demo) | Vercel / GitHub Pages |

**Why offline-first?** Every core feature (tracking, alerts, guidance, reports) must work with **zero network access**. Data lives on the device, and nothing depends on a server.

---

## Project Structure

```
astracare/
├── public/
│   ├── manifest.json
│   └── icons/
├── src/
│   ├── components/        # Reusable UI components
│   ├── pages/             # Dashboard, Track, Guide, Alerts, Reports, Emergency
│   ├── data/
│   │   ├── guidance/      # Offline medical guidance content (JSON/Markdown)
│   │   └── ranges.ts      # Reference ranges for vitals
│   ├── engine/            # Alert / analysis logic
│   ├── db/                # IndexedDB schema and helpers
│   ├── reports/           # Report generation and export
│   └── utils/
├── tests/
├── docs/                  # Pitch deck, diagrams, screenshots
└── README.md
```

---

## Getting Started

### Prerequisites

- Node.js 18+
- npm or pnpm

### Installation

```bash
git clone https://github.com/<your-org>/astracare.git
cd astracare
npm install
npm run dev
```

The app runs at `http://localhost:5173`.

### Build and test offline mode

```bash
npm run build
npm run preview
```

Open the preview, load the app once, then switch your browser to **Offline** (DevTools, Network tab) and confirm everything still works.

---

## Team and Task Division

> Replace the names below with your team's. **Working with 5 people?** Merge Role 6 into Roles 1 and 4: the Project Lead takes the pitch and docs, and the Alerts Engineer takes testing.

| # | Role | Owner | Responsibilities | Main deliverables |
|---|---|---|---|---|
| 1 | **Project Lead and Pitch** | `@name` | Keeps scope under control, manages the board and merges, owns the demo script and pitch deck, writes docs | README, pitch deck, demo script, final demo |
| 2 | **Frontend and UX Lead** | `@name` | App layout, navigation, design system, dashboard, responsive/mobile view, space-themed UI | UI kit, Dashboard page, navigation |
| 3 | **Vitals Tracking (Track)** | `@name` | Forms for heart rate, blood pressure, sleep, mood and symptoms; history view; trend charts | Track page, charts, input validation |
| 4 | **Alerts Engineer (Alert)** | `@name` | Reference ranges, analysis logic, warning levels (normal, caution, urgent), trend detection | Alert engine, unit tests, alert banners |
| 5 | **Medical Content and Emergency (Guide and Respond)** | `@name` | Research and write offline guidance from reputable sources, build step-by-step emergency procedures, searchable library | Guidance library, Emergency page, source list |
| 6 | **Offline, Data and Reports (Report)** | `@name` | PWA setup, service worker, IndexedDB storage, report generation and export, QA of offline mode | Offline support, Report page, export, QA checklist |

### How the pieces connect

- **Role 3 → Role 6**: vitals are saved to the IndexedDB schema Role 6 defines. *Agree on the data schema on Day 1.*
- **Role 3 → Role 4**: the alert engine reads saved readings and returns an alert level.
- **Role 4 → Role 2**: alert levels drive the colours and banners on the dashboard.
- **Role 4 → Role 5**: each alert links to the matching guidance or emergency procedure.
- **Role 5 → Role 6**: reports can include the relevant guidance that was accessed.

### Shared responsibilities

- Everyone reviews at least one pull request per day.
- Everyone tests the offline mode before their feature is marked done.
- Everyone contributes to the demo rehearsal.

---

## Hackathon Timeline

| Phase | Goal | Who |
|---|---|---|
| **1. Setup** | Repo, tooling, data schema, design direction, task board | Everyone (Lead drives) |
| **2. Build core** | Track page, alert engine, guidance content, PWA shell | Roles 2 to 6 in parallel |
| **3. Integrate** | Connect alerts to guidance, dashboard to live data, reports | Everyone |
| **4. Polish and test** | Offline testing, bug fixes, UI polish, sample data for the demo | Everyone |
| **5. Present** | Pitch deck final, demo rehearsal (run it at least twice) | Lead and everyone |

**Tip:** freeze new features about 20% of the time before the deadline. After that, only fix bugs and polish.

---

## Git Workflow

- `main` is always demo-ready. Never push directly to it.
- Create a branch per task: `feature/<role>-<short-name>`, for example `feature/alerts-bp-ranges`.
- Open a pull request and get **one approval** before merging.
- Use clear commits, such as `feat: add heart rate input form` or `fix: sleep chart crash`.
- Keep pull requests small and merge often to avoid conflicts.

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
| _Name_ | Project Lead and Pitch | [@username](https://github.com/username) |
| _Name_ | Frontend and UX Lead | [@username](https://github.com/username) |
| _Name_ | Vitals Tracking | [@username](https://github.com/username) |
| _Name_ | Alerts Engineer | [@username](https://github.com/username) |
| _Name_ | Medical Content and Emergency | [@username](https://github.com/username) |
| _Name_ | Offline, Data and Reports | [@username](https://github.com/username) |

---

<p align="center"><i>"No matter the distance, health comes first."</i></p>
