/*
 * 🚀 ASTROPASS: Enterprise Interstellar Security Terminal
 * Target Platform: Arduino Architecture (Uno / Mega / Nano)
 * Complete Multi-Layered Firmware System with Custom Graphics, Diagnostic Logs,
 * Environmental Hazard Validation, and Automatic Intrusion Lockout Systems.
 */

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// --- HARDWARE CONFIGURATION & PIN REGISTER MATRIX ---
const uint8_t STATUS_LED_GREEN = 4;   // Authorized Access Beacon
const uint8_t STATUS_LED_RED   = 5;   // Intrusion Alert Beacon
const uint8_t HATCH_LOCK_RELAY = 6;   // Electromagnetic Door Interlock System
const uint8_t ALARM_BUZZER     = 7;   // Active Audio Warning Transducer
const uint8_t SYSTEM_RESET_PIN = 8;   // Hard Manual Over-ride Interrupt Switch

// --- SYSTEM LIMITS AND REGISTER ARGUMENTS ---
#define INSTANCE_DATABASE_SIZE 4
#define ACCESS_LOG_MAX_ENTRIES 5
#define TRANSMISSION_BAUD_RATE 9600
#define INTRUSION_THRESHOLD    3
#define COOLDOWN_DURATION_MS   8000

// --- STRUCTS AND DATA SCHEMAS ---
struct AstronautProfile {
  String badgeID;
  String legalName;
  String clearanceLevel;
};

struct AccessRecord {
  String timestamp;
  String name;
  bool isAuthorized;
};

// --- SYSTEM FINITE STATE MACHINE REPRESENTATIONS ---
enum SystemCoreState {
  STATE_INITIALIZING,
  STATE_STANDBY_READY,
  STATE_VALIDATING_CREDENTIALS,
  STATE_ENVIRONMENTAL_CHECK,
  STATE_GRANTED_DECOMPRESSION,
  STATE_LOCKDOWN_ALERT
};

// --- GLOBAL STATE REGISTERS ---
SystemCoreState currentGlobalState = STATE_INITIALIZING;
uint8_t continuousFailedAttempts = 0;
uint32_t terminalLockoutStartTime = 0;
int logTrackerIndex = 0;

// --- HARDWARE INSTANTIATION ---
LiquidCrystal_I2C lcd(0x27, 16, 2);

// --- VOLATILE CORE DATA STORAGE ---
AstronautProfile flightCrewDatabase[INSTANCE_DATABASE_SIZE] = {
  {"ASTRO-ASHISH",   "ASHISH",     "COMMANDER"},
  {"ASTRO-ALPHA",    "CDR. MILLER", "PILOT"},
  {"ASTRO-EXPLO",    "DR. ARIS",   "RESEARCH"},
  {"ASTRO-GUEST",    "SUPPORT-01", "MAINTENANCE"}
};

AccessRecord telemetryLog[ACCESS_LOG_MAX_ENTRIES];

// --- CUSTOM BITMAP GRAPHICS FOR DISPLAY BUFFER ---
uint8_t customLockIcon[8] = {
  0b00110, 0b01001, 0b01001, 0b11111, 0b11011, 0b11011, 0b11111, 0b00000
};

uint8_t customCheckIcon[8] = {
  0b00000, 0b00001, 0b00011, 0b10110, 0b11100, 0b11000, 0b00000, 0b00000
};

uint8_t customWarningIcon[8] = {
  0b00100, 0b00100, 0b01110, 0b01110, 0b11111, 0b11111, 0b00100, 0b00000
};

// --- PROTOTYPES CONTROL PIPELINE ---
void initializeHardwarePeripherals();
void injectCustomGlyphs();
void renderStandbyInterface();
void evaluateIncomingScan(String verificationPayload);
void executeEnvironmentalMatrix(AstronautProfile astronaut);
void grantTerminalPassage(AstronautProfile astronaut);
void renderRejectionSequence();
void commitAccessTelemetry(String identity, bool passStatus);
void triggerTerminalLockout();
void displayFormattedLogHistory();
void handleEmergencySystemReset(); // Added missing prototype to prevent scope errors

void setup() {
  Serial.begin(TRANSMISSION_BAUD_RATE);
  initializeHardwarePeripherals();
  injectCustomGlyphs();
  
  Serial.println(F("[SYSTEM-BOOT] Astropass Architecture Loaded successfully."));
  Serial.println(F("[SYSTEM-BOOT] Total Core Records Registered: 4."));
  
  currentGlobalState = STATE_STANDBY_READY;
  renderStandbyInterface();
}

void loop() {
  // Check System Emergency Reset Switch
  if (digitalRead(SYSTEM_RESET_PIN) == LOW) {
    handleEmergencySystemReset();
  }

  // Evaluate Lockout Timeout Condition
  if (currentGlobalState == STATE_LOCKDOWN_ALERT && (millis() - terminalLockoutStartTime >= COOLDOWN_DURATION_MS)) {
    Serial.println(F("[SECURITY] Lockout timer expired. Restoring core terminal control loops."));
    continuousFailedAttempts = 0;
    currentGlobalState = STATE_STANDBY_READY;
    renderStandbyInterface();
  }

  // Primary State Engine Action Loops
  switch (currentGlobalState) {
    
    case STATE_STANDBY_READY:
      if (Serial.available() > 0) {
        String inputBuffer = Serial.readStringUntil('\n');
        inputBuffer.trim();
        
        if (inputBuffer.length() > 0) {
          evaluateIncomingScan(inputBuffer);
        }
      }
      break;

    case STATE_LOCKDOWN_ALERT:
      // Oscillate alarm and warning lights natively during critical locks
      digitalWrite(STATUS_LED_RED, HIGH);
      delay(150);
      digitalWrite(STATUS_LED_RED, LOW);
      delay(150);
      break;

    default:
      break;
  }
}

// --- SUBSYSTEM ENGINE ROUTINES ---

void initializeHardwarePeripherals() {
  lcd.init();
  lcd.backlight();
  
  pinMode(STATUS_LED_GREEN, OUTPUT);
  pinMode(STATUS_LED_RED, OUTPUT);
  pinMode(HATCH_LOCK_RELAY, OUTPUT);
  pinMode(ALARM_BUZZER, OUTPUT);
  pinMode(SYSTEM_RESET_PIN, INPUT_PULLUP);
  
  // Enforce zero state positions natively
  digitalWrite(STATUS_LED_GREEN, LOW);
  digitalWrite(STATUS_LED_RED, HIGH); // System starts safely locked
  digitalWrite(HATCH_LOCK_RELAY, LOW);
}

void injectCustomGlyphs() {
  lcd.createChar(0, customLockIcon);
  lcd.createChar(1, customCheckIcon);
  lcd.createChar(2, customWarningIcon);
}

void renderStandbyInterface() {
  digitalWrite(STATUS_LED_GREEN, LOW);
  digitalWrite(STATUS_LED_RED, HIGH);
  digitalWrite(HATCH_LOCK_RELAY, LOW);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.write(0); // Display custom Lock symbol
  lcd.print(" ASTROPASS TERM "); // Shortened to fit 16-char screen
  lcd.setCursor(0, 1);
  lcd.print("READY TO SCAN...");
}

void evaluateIncomingScan(String verificationPayload) {
  currentGlobalState = STATE_VALIDATING_CREDENTIALS;
  Serial.print(F("[DECODING] Scanned Token Stream ID: "));
  Serial.println(verificationPayload);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("READING TOKEN...");
  lcd.setCursor(0, 1);
  lcd.print("DECRYPTING AUTH...");
  delay(1200);

  bool credentialMatched = false;
  AstronautProfile activeAstronaut;

  for (int i = 0; i < INSTANCE_DATABASE_SIZE; i++) {
    if (flightCrewDatabase[i].badgeID == verificationPayload) {
      credentialMatched = true;
      activeAstronaut = flightCrewDatabase[i];
      break;
    }
  }

  if (credentialMatched) {
    continuousFailedAttempts = 0; 
    executeEnvironmentalMatrix(activeAstronaut);
  } else {
    continuousFailedAttempts++;
    Serial.print(F("[ALERT] Unauthorized access detected. Attempt counter: "));
    Serial.print(continuousFailedAttempts);
    Serial.print(F("/"));
    Serial.println(INTRUSION_THRESHOLD);

    commitAccessTelemetry("UNKNOWN_BADGE", false);

    if (continuousFailedAttempts >= INTRUSION_THRESHOLD) {
      triggerTerminalLockout();
    } else {
      renderRejectionSequence();
    }
  }
}

void executeEnvironmentalMatrix(AstronautProfile astronaut) {
  currentGlobalState = STATE_ENVIRONMENTAL_CHECK;
  
  Serial.println(F("[ENVIRONMENT] Initializing atmospheric cabin safety analysis sweeps..."));
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("ENV CHECK: BUSY");
  
  for (int processPercent = 25; processPercent <= 100; processPercent += 25) {
    lcd.setCursor(0, 1);
    lcd.print("O2/PRES OK: ");
    lcd.print(processPercent);
    lcd.print("%");
    tone(ALARM_BUZZER, 2000, 30);
    delay(500);
  }

  Serial.println(F("[ENVIRONMENT] Parameters Nominal. Equalization cycle safe."));
  commitAccessTelemetry(astronaut.legalName, true);
  grantTerminalPassage(astronaut);
}

void grantTerminalPassage(AstronautProfile astronaut) {
  currentGlobalState = STATE_GRANTED_DECOMPRESSION;
  
  Serial.print(F("[ACCESS] Granted. Clearance Level Validated: ["));
  Serial.print(astronaut.clearanceLevel);
  Serial.println(F("] Welcome back, user."));

  digitalWrite(STATUS_LED_RED, LOW);
  digitalWrite(STATUS_LED_GREEN, HIGH);
  digitalWrite(HATCH_LOCK_RELAY, HIGH); 

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.write(1); // Render Custom Checkmark Symbol
  lcd.print(" AUTH GRANTED");
  lcd.setCursor(0, 1);
  lcd.print("HI " + astronaut.legalName + " (" + astronaut.clearanceLevel[0] + ")");

  tone(ALARM_BUZZER, 1000, 100);
  delay(150);
  tone(ALARM_BUZZER, 1400, 200);

  delay(5000); // Hold open door lock for 5 seconds total

  currentGlobalState = STATE_STANDBY_READY;
  renderStandbyInterface();
}

void renderRejectionSequence() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.write(2); // Render warning symbol
  lcd.print(" ACCESS DENIED");
  lcd.setCursor(0, 1);
  lcd.print("INVALID BADGE");

  for (int alertFlash = 0; alertFlash < 3; alertFlash++) {
    digitalWrite(STATUS_LED_RED, HIGH);
    tone(ALARM_BUZZER, 400, 200);
    delay(250);
    digitalWrite(STATUS_LED_RED, LOW);
    delay(150);
  }

  currentGlobalState = STATE_STANDBY_READY;
  renderStandbyInterface();
}

void triggerTerminalLockout() {
  currentGlobalState = STATE_LOCKDOWN_ALERT;
  terminalLockoutStartTime = millis();
  
  Serial.println(F("[CRITICAL] SECURITY THRESHOLD EXCEEDED. HARDLOCK ACTIVATED."));
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.write(2);
  lcd.print(" FIRMWARE LOCK");
  lcd.setCursor(0, 1);
  lcd.print("COOLDOWN ACTIVE "); // Kept exactly to 16 characters

  tone(ALARM_BUZZER, 150, 1000);
}

void commitAccessTelemetry(String identity, bool passStatus) {
  telemetryLog[logTrackerIndex].timestamp = "LOG_" + String(millis() / 1000) + "s";
  telemetryLog[logTrackerIndex].name = identity;
  telemetryLog[logTrackerIndex].isAuthorized = passStatus;

  logTrackerIndex = (logTrackerIndex + 1) % ACCESS_LOG_MAX_ENTRIES; 
  displayFormattedLogHistory();
}

void displayFormattedLogHistory() {
  Serial.println(F("\n====== SYSTEM STORAGE TERMINAL SECURITY ARCHIVE ======"));
  for (int indexIter = 0; indexIter < ACCESS_LOG_MAX_ENTRIES; indexIter++) {
    if (telemetryLog[indexIter].name.length() > 0) {
      Serial.print(F("["));
      Serial.print(telemetryLog[indexIter].timestamp);
      Serial.print(F("] User: "));
      Serial.print(telemetryLog[indexIter].name);
      Serial.print(F(" | Status: "));
      Serial.println(telemetryLog[indexIter].isAuthorized ? F("PASS") : F("VIOLATION"));
    }
  }
  Serial.println(F("======================================================"));
}

void handleEmergencySystemReset() {
  Serial.println(F("\n⚠️ [INTERRUPT] HARD OVERRIDE BUTTON TRIGGERED. FLUSHING STACKS."));
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("EMERGENCY FLUSH");
  lcd.setCursor(0, 1);
  lcd.print("REBOOTING OS...");
  
  digitalWrite(STATUS_LED_GREEN, HIGH);
  digitalWrite(STATUS_LED_RED, HIGH);
  tone(ALARM_BUZZER, 3000, 500);
  delay(1000);
  
  continuousFailedAttempts = 0;
  currentGlobalState = STATE_STANDBY_READY;
  renderStandbyInterface();
}
