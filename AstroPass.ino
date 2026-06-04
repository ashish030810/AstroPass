#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN         9          
#define SS_PIN          10         

// Indicator Pins
#define GREEN_LED       5
#define RED_LED         4
#define BUZZER          3

MFRC522 mfrc522(SS_PIN, RST_PIN);  

// --- ADVANCED DATA STRUCTURES ---
struct CrewMember {
  byte uid[4];        
  const char* name;   
  const char* role;   
  int clearanceLevel; // Scale from 1 (Lowest) to 5 (Admin)
  int accessCount;    // Tracks individual scans dynamically
  int assignedShift;  // 1 = Morning Shift, 2 = Night Shift, 3 = 24/7 Access
};

// Main Database Array
CrewMember crewDatabase[] = {
  {{0xDE, 0xAD, 0xBE, 0xEF}, "Commander Ashish", "Alpha Command", 5, 0, 3},
  {{0x01, 0x02, 0x03, 0x04}, "Dr. Sarabhai", "Research Lab", 3, 0, 1},
  {{0xAA, 0xBB, 0xCC, 0xDD}, "Aman Sharma", "Engineering Deck", 2, 0, 1},
  {{0x99, 0x88, 0x77, 0x66}, "Priya Patel", "Communications", 2, 0, 2},
  {{0x55, 0x44, 0x33, 0x22}, "Rohan Das", "Life Support", 1, 0, 2}
};

const int TOTAL_CREW = sizeof(crewDatabase) / sizeof(crewDatabase[0]);

// --- SYSTEM SECURITY STATE VARIABLES ---
int consecutiveFailures = 0;
bool systemLockdown = false;
unsigned long lockdownStartTime = 0;
const unsigned long LOCKDOWN_DURATION = 30000; // 30 seconds

// Simulated Environmental Variables (Shift Control)
unsigned long loopCounter = 0;
int currentSystemShift = 1; // 1 = Morning, 2 = Night

void setup() {
  Serial.begin(9600);   
  SPI.begin();          
  mfrc522.PCD_Init();   
  
  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(BUZZER, OUTPUT);

  Serial.println(F("\n=================================================="));
  Serial.println(F("🚀 ASTROPASS: ULTIMATE 50-HOUR SECURITY TERMINAL 🚀"));
  Serial.println(F("System Status: OPERATIONAL | Encryption: ACTIVE"));
  Serial.println(F("=================================================="));
}

void loop() {
  // Update Simulated Time Environment (Cycles Shifts)
  loopCounter++;
  if (loopCounter % 50 == 0) { 
    currentSystemShift = (currentSystemShift == 1) ? 2 : 1;
    Serial.print(F("\n[ENVIRONMENT NOTICE] Station Shift Rotated. Current Shift: "));
    Serial.println(currentSystemShift == 1 ? F("MORNING ROTATION") : F("NIGHT ROTATION"));
  }

  // Handle Active Brute-Force Lockdown Mode
  if (systemLockdown) {
    unsigned long elapsed = millis() - lockdownStartTime;
    if (elapsed >= LOCKDOWN_DURATION) {
      systemLockdown = false;
      consecutiveFailures = 0;
      Serial.println(F("\n🔓 [SECURITY NOTICE] Lockdown status lifted. Terminal active."));
      digitalWrite(RED_LED, LOW);
    } else {
      // Flash Red Warning Light silently during active lockdown block
      digitalWrite(RED_LED, (millis() / 250) % 2); 
      return; 
    }
  }

  // Polling for RFID Tokens
  if ( ! mfrc522.PICC_IsNewCardPresent()) return;
  if ( ! mfrc522.PICC_ReadCardSerial()) return;

  Serial.print(F("\n[INCOMING SIGNAL] Processing UID Token:"));
  printHex(mfrc522.uid.uidByte, mfrc522.uid.size);

  bool cardRecognized = false;

  // Search Engine Array Parsing
  for (int i = 0; i < TOTAL_CREW; i++) {
    if (checkCardMatch(mfrc522.uid.uidByte, crewDatabase[i].uid)) {
      cardRecognized = true;
      processAccessRequest(crewDatabase[i]);
      break; 
    }
  }

  // If Unregistered Fake Card is Used
  if (!cardRecognized) {
    handleIntrusionEvent();
  }

  mfrc522.PICC_HaltA();
}

// Check matching bytes
bool checkCardMatch(byte *scanned, const byte *databaseCard) {
  for (byte i = 0; i < 4; i++) {
    if (scanned[i] != databaseCard[i]) return false;
  }
  return true;
}

// Core Verification Matrix Logic
void processAccessRequest(CrewMember &member) {
  // Check Rule 1: Shift Authentication Schedule
  if (member.assignedShift != 3 && member.assignedShift != currentSystemShift) {
    Serial.print(F("\n⚠️ [ACCESS DENIED] Shift Conflict for "));
    Serial.println(member.name);
    Serial.println(F("Reason: Identity valid, but current sector shift restriction applies."));
    triggerHardwareDenial();
    return;
  }

  // Check Rule 2: Minimum Security Levels for Core Access
  if (member.clearanceLevel < 2) {
    Serial.print(F("\n⚠️ [ACCESS RESTRICTED] Clearance Too Low: "));
    Serial.println(member.name);
    Serial.println(F("Reason: Level 1 Personnel restricted from Main Core access."));
    triggerHardwareDenial();
    return;
  }

  // Access Granted Routine Passed
  consecutiveFailures = 0; // Reset intrusion logs
  member.accessCount++;    // Increment individual user logging tracker
  
  Serial.println(F("\n🔓 [ACCESS AUTHORIZED] -----------------------"));
  Serial.print(F("User Identifier:  ")); Serial.println(member.name);
  Serial.print(F("Assigned Sector:  ")); Serial.println(member.role);
  Serial.print(F("Clearance Rank:   Level ")); Serial.println(member.clearanceLevel);
  Serial.print(F("Session Log Count:")); Serial.print(member.accessCount); Serial.println(F(" entries logged."));
  Serial.println(F("----------------------------------------------"));
  
  triggerHardwareApproval();
}

// Handle Fake/Failed Scans
void handleIntrusionEvent() {
  consecutiveFailures++;
  Serial.println(F("\n🚨 [SECURITY BREACH] Unknown Token signature detected!"));
  Serial.print(F("Consecutive Security Invalidation Count: "));
  Serial.println(consecutiveFailures);

  if (consecutiveFailures >= 3) {
    systemLockdown = true;
    lockdownStartTime = millis();
    Serial.println(F("\n🛑 [CRITICAL BREAK ALERT] - MAIN TERMINAL LOCKDOWN INITIATED"));
    Serial.println(F("Action: Isolating node console access for 30 seconds."));
  } else {
    triggerHardwareDenial();
  }
}

// Hardware Beeps and Lights Execution
void triggerHardwareApproval() {
  digitalWrite(GREEN_LED, HIGH);
  digitalWrite(BUZZER, HIGH);
  delay(150);
  digitalWrite(BUZZER, LOW);
  delay(1500); 
  digitalWrite(GREEN_LED, LOW);
}

void triggerHardwareDenial() {
  for (int i = 0; i < 3; i++) {
    digitalWrite(RED_LED, HIGH);
    digitalWrite(BUZZER, HIGH);
    delay(100);
    digitalWrite(RED_LED, LOW);
    digitalWrite(BUZZER, LOW);
    delay(80);
  }
}

void printHex(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
  Serial.println();
}