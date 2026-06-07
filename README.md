# 🚀 ASTROPASS: Enterprise Interstellar Security Terminal

An advanced, multi-layered embedded security terminal engineered for the Hack Club x NASA Stardance Challenge. This system simulates a spacecraft hatch control interlock system featuring custom LCD graphics, automated environmental cabin validation sweeps, an access-log history tracker, and an anti-intrusion automatic lockout module.

---

## 🛠️ Hardware Pin Configuration Matrix

The physical architecture is wired using the following pin map:

| Component | Arduino Uno Pin | Description |
| :--- | :--- | :--- |
| **I2C LCD (SDA)** | **A4** | Data Line for 16x2 Display |
| **I2C LCD (SCL)** | **A5** | Clock Line for 16x2 Display |
| **Green LED** | **Pin 4** | Authorized Access Beacon |
| **Red LED** | **Pin 5** | Intrusion Alert Beacon / System Safe Lock |
| **Hatch Relay** | **Pin 6** | Electromagnetic Door Interlock Simulator |
| **Buzzer** | **Pin 7** | Active Audio Warning Transducer |
| **Reset Switch** | **Pin 8** | Hard Manual Over-ride Interrupt Button |

---

## 🛰️ Core System Features

* **Finite State Machine Architecture:** Avoids processor-blocking delays by utilizing an optimized `switch-case` architecture tracking system states natively.
* **Custom LCD Glyphs:** Loads custom-designed binary bitmaps (Padlock, Verification Checkmark, and Alert Triangle) directly into the display hardware buffer.
* **Environmental Life Support Sweep:** Simulates automated atmospheric testing cycles ($O_2$ and pressure checks) before allowing decompression entry.
* **Automated Intrusion Defense:** Features a 3-strike penalty threshold that instantly initiates a hard-lock cooldown timer (`8000ms`) upon consecutive credential failures.
* **Circular Buffer Log System:** Tracks runtime event histories and outputs formatted diagnostic log tables over the Serial data stream.

---

## 📦 How It Works

1. **Standby Mode:** The terminal defaults to a secure locked state (Red LED on) and renders `READY TO SCAN...` alongside a custom lock graphic.
2. **Scan Evaluation:** Passing an authorization badge ID (e.g., `ASTRO-ASHISH`) through the Serial Monitor triggers a token decryption sequence.
3. **Safety Analysis:** If the badge matches the database profile, the terminal runs a dynamic life-support diagnostic sweep from 0% to 100%.
4. **Access Granted:** The system turns on the Green LED, flips the magnetic door relay, and displays a personalized welcome message (`WELCOME back, user`).
5. **Intrusion Trigger:** Unrecognized credentials flash warning screens and sound acoustic chimes. 3 failures activate the system lockdown.

---

## 🪵 Project Milestones & Time Log
* **Total Time Logged:** 11 Hours & 8 Minutes via Hackatime.
* **Version:** 1.0 Complete Release.
