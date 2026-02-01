#include <SPI.h>
#include <MFRC522.h>

// ---------------- RFID CONFIG ----------------
#define SS_PIN1 10
#define RST_PIN1 9
#define SS_PIN2 8
#define RST_PIN2 7

MFRC522 entryRFID(SS_PIN1, RST_PIN1); // Entry reader
MFRC522 exitRFID(SS_PIN2, RST_PIN2);  // Exit reader

// ---------------- SIGNAL LED CONFIG ----------------
// 4 signals (Red + Green). 3 signals also have Yellow.
int redLED[4]    = {2, 4, 6, A0};
int greenLED[4]  = {3, 5, A1, A2};
int yellowLED[3] = {A3, A4, A5}; // Only 3 yellows (Signal 0,1,2)

// ---------------- AMBULANCE TAGS ----------------
String allowedUIDs[] = {
  "04A1B2C3",   // Ambulance 1
  "12FF34AB",   // Ambulance 2
  "98BC76D4"    // Ambulance 3
};
int totalTags = 3;

// ---------------- VARIABLES ----------------
int currentSignal = 0;
unsigned long cycleTime = 5000;
unsigned long lastSwitch = 0;
bool ambulanceActive = false;

// Flexible mapping: choose which signal Entry RFID controls
int entrySignalID = 0;   // Change to 1,2,3 if needed

// ---------------- SETUP ----------------
void setup() {
  Serial.begin(9600);
  SPI.begin();
  entryRFID.PCD_Init();
  exitRFID.PCD_Init();

  // Disable both RFID readers initially
  pinMode(SS_PIN1, OUTPUT);
  pinMode(SS_PIN2, OUTPUT);
  digitalWrite(SS_PIN1, HIGH);
  digitalWrite(SS_PIN2, HIGH);

  for (int i = 0; i < 4; i++) {
    pinMode(redLED[i], OUTPUT);
    pinMode(greenLED[i], OUTPUT);
  }
  for (int i = 0; i < 3; i++) {
    pinMode(yellowLED[i], OUTPUT);
  }

  setSignal(0); // Start with Signal 0
}

// ---------------- LOOP ----------------
void loop() {

  // Check Entry RFID → Activate ambulance mode
  if (!ambulanceActive && checkRFID(entryRFID, SS_PIN1, true)) {
    ambulanceActive = true;
  }

  // Check Exit RFID → Resume normal cycle
  if (ambulanceActive && checkRFID(exitRFID, SS_PIN2, false)) {
    ambulanceActive = false;
    lastSwitch = millis();
    setSignal(currentSignal);
  }

  // Normal traffic cycle
  if (!ambulanceActive) {
    if (millis() - lastSwitch > cycleTime) {
      int prevSignal = currentSignal;
      currentSignal = (currentSignal + 1) % 4;

      if (prevSignal < 3) {
        digitalWrite(redLED[prevSignal], LOW);
        digitalWrite(greenLED[prevSignal], LOW);
        digitalWrite(yellowLED[prevSignal], HIGH);
        delay(2000);
        digitalWrite(yellowLED[prevSignal], LOW);
      }

      setSignal(currentSignal);
      lastSwitch = millis();
    }
  }
}

// ---------------- FUNCTIONS ----------------

// Set one signal GREEN, others RED
void setSignal(int signal) {
  for (int i = 0; i < 4; i++) {
    if (i == signal) {
      digitalWrite(greenLED[i], HIGH);
      digitalWrite(redLED[i], LOW);
      Serial.print("Signal ");
      Serial.print(i);
      Serial.println(" -> GREEN");
    } else {
      digitalWrite(greenLED[i], LOW);
      digitalWrite(redLED[i], HIGH);
    }
  }
}

// RFID verification function
bool checkRFID(MFRC522 &rfid, int ssPin, bool isEntry) {
  digitalWrite(ssPin, LOW);  // Enable reader
  bool success = false;

  if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
    String scannedUID = "";

    for (byte i = 0; i < rfid.uid.size; i++) {
      if (rfid.uid.uidByte[i] < 0x10) scannedUID += "0";
      scannedUID += String(rfid.uid.uidByte[i], HEX);
    }
    scannedUID.toUpperCase();

    Serial.print("Scanned UID: ");
    Serial.println(scannedUID);

    for (int i = 0; i < totalTags; i++) {
      if (scannedUID == allowedUIDs[i]) {
        if (isEntry) {
          Serial.println("Ambulance detected at ENTRY");
          currentSignal = entrySignalID;
          setSignal(currentSignal);
        } else {
          Serial.println("Ambulance EXIT detected");
        }
        success = true;
        break;
      }
    }

    if (!success) {
      Serial.println("Unauthorized RFID tag");
    }

    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
  }

  digitalWrite(ssPin, HIGH); // Disable reader
  return success;
}
