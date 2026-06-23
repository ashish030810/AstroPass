# ASTROPASS
Smart RFID crew access system — because space doesn't forgive unauthorized entry

## What is this?

Imagine you're on Mars or the Moon. There's a habitat — maybe a lab, maybe a life support room. You can't just let anyone walk in. The wrong person entering at the wrong time could be catastrophic.

ASTROPASS is a prototype entry/exit security terminal built for exactly that. Each astronaut gets their own RFID tag. Scan it — if it's registered, you're in. If it's not, the system locks you out. No exceptions, no overrides without the right credentials.

I built this for the Hack Club x NASA Stardance Challenge as a real working prototype of what crew access control could look like on a deep space mission.

## How it works

The flow is simple but the logic underneath isn't:

1. System boots into standby — red LED on, LCD shows `READY TO SCAN`
2. Astronaut scans their RFID tag on the MFRC522 reader
3. If the tag matches a registered crew ID → green LED, relay opens, welcome message
4. If it doesn't match → buzzer sounds, strike logged
5. 3 failed scans in a row → full lockdown for 8 seconds, no entry possible

Before granting access, it also runs an automated life-support sweep — simulating O₂ and pressure checks from 0% to 100%. The hatch doesn't open until that passes too.

## Hardware

- Arduino Uno
- MFRC522 RFID reader + crew tags
- I2C 16x2 LCD display
- Green LED (access granted) + Red LED (locked/alert)
- Buzzer (intrusion alert)
- Relay module (hatch interlock)
- Reset button (manual override)

## What makes it different

- No blocking delays — runs on a finite state machine so the system stays responsive at all times
- Custom LCD graphics — I designed the padlock, checkmark and alert icons as binary bitmaps loaded directly into the display buffer
- Every scan is logged — circular buffer keeps a history of all access events, readable over Serial
- 3-strike lockout — consecutive failures trigger a hard cooldown, just like a real secure terminal


---
Built by Ashish — Hack Club x NASA Stardance Challenge, 2026
  
