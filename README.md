# School Safety Tracker (IoT + Real-Time Monitoring System)

An intelligent indoor safety monitoring system designed to track student locations and detect proximity to **Danger Zones** using BLE signal processing and real-time data visualization.

---

## 🚀 Overview
This system utilizes a distributed sensor network to monitor movements in a classroom environment. By processing **RSSI (Received Signal Strength Indication)** through a **Kalman Filter**, the system provides smoothed distance estimations to ensure accurate safety alerts.

### Core Stack:
- **Embedded:** ESP32 (BLE + WiFi)
- **Signal Processing:** Kalman Filtering (Noise Reduction)
- **Backend:** FastAPI (Python) + WebSockets
- **Frontend:** Real-time Streamlit/HTML Dashboard

---

## 🏗️ System Architecture
`ESP32 (Student Device)` → `BLE Scan (RSSI)` → `Kalman Filter` → `Distance Estimation` → `WiFi (POST)` → `FastAPI Backend` → `WebSocket` → `Live Dashboard`

---

## 🛠️ Hardware & Firmware
### Student Node (ESP32)
- Continuous BLE scanning of localized Beacons.
- Real-time RSSI smoothing to handle signal multipath interference.
- Logic: Triggers an SOS/Alert when `Danger_Zone_Distance < Safety_Zone_Threshold`.

### Beacon Nodes
- Fixed BLE transmitters representing safe zones (Classrooms) and restricted zones (Danger Zones).

---

## 💻 Software Implementation
### Backend (`main.py`)
- **FastAPI:** High-performance asynchronous API to handle high-frequency sensor updates.
- **WebSockets:** Bi-directional communication for zero-latency dashboard updates.
- **Data Flow:** Receives sensor payloads and pushes them to the frontend map visualization.

### Frontend Dashboard (`index.html`)
- **Real-time Map:** Dynamic visualization of student movement.
- **Visual Cues:** Green zones (Safe) and Red zones (Danger).
- **Alert System:** Instant pop-ups and buzzer triggers based on proximity logic.

---

## 📊 Key Results & Features
- **Noise Mitigation:** Integrated Kalman Filtering to stabilize erratic RSSI data.
- **Low Latency:** Achieved ~1.5s end-to-end update frequency.
- **Modular Design:** Easily scalable to include more students or larger facility maps.

---

## 🚀 Roadmap: Toward Embodied AI
This project serves as a foundational step toward a comprehensive **Smart Environment Intelligence** system.

### Phase 1: Advanced Localization (Short-Term)
- Implement **Multi-beacon Trilateration** for precise $X, Y$ coordinates.
- Transition to **Sensor Fusion** (IMU + BLE) for robust indoor navigation.

### Phase 2: Computer Vision Integration (Medium-Term)
- Integrate **YOLOv8** to validate "Danger Zone" breaches via camera feed.
- Combine IoT sensor data with visual confirmation for zero false-alarm rates.

### Phase 3: Embodied AI & VLM (Long-Term)
- **Vision-Language Models (VLM):** Enabling the system to understand scene context (e.g., "Student is running near a wet floor").
- **Autonomous Monitoring:** Deploying mobile robots to patrol identified risk areas using the shared sensor map.

---

## 📁 Repository Structure
```text
School-Safety-Tracker
│
├── Hardware
│   ├── child_esp32_code.ino
│   └── beacon_node_code.ino
│
├── Software
│   ├── main.py          # FastAPI Backend
│   ├── index.html       # Real-time Dashboard
│   └── models.py        # Data Schemas
│
├── Assets
│   ├── GUI_Main.png
│   └── Live_Tracking.png
└── README.md
