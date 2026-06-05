# GestureTune — Touchless Music Controller

> **An HCI (Human-Computer Interaction) Project**  
> American International University-Bangladesh (AIUB)

---

## Overview

**GestureTune** is a touchless music player controlled entirely by hand gestures. Using an **Arduino UNO**, **DFPlayer Mini**, and **four HC-SR04 ultrasonic sensors**, it detects hand movements to control music playback — no physical buttons or screens required.

| Team Member | ID |
|---|---|
| Tutul Kumar Ghosh | 22-47813-2 |
| P.M Tasriful Islam | 22-47885-2 |
| Ananya Dutta | 22-47966-2 |
| Md. Saif Ahmed Sourav | 22-46790-2 |

---

## Features

| Gesture | Action |
|---|---|
| Hand close to Sensor 1 (1–10 cm) | **Pause** |
| Hand far from Sensor 1 (11–20 cm) | **Play** |
| Hand close to Sensor 2 (1–10 cm) | **Next Track** |
| Hand far from Sensor 2 (11–20 cm) | **Previous Track** |
| Hand close to Sensor 3 (1–10 cm) | **Volume Up** |
| Hand close to Sensor 4 (1–10 cm) | **Volume Down** |

- Real-time distance debugging via Serial Monitor
- Debounce delays to prevent accidental triggers
- Initial volume: 20 (range: 0–30)
- Supports all MP3 files on a microSD/TF card

---

## Hardware Requirements

| Component | Quantity |
|---|---|
| Arduino UNO | 1 |
| DFPlayer Mini (MP3/TF module) | 1 |
| HC-SR04 Ultrasonic Sensor | 4 |
| Speaker (3W–5W, 4Ω or 8Ω) | 1 |
| microSD/TF card (with MP3 files) | 1 |
| Breadboard & Jumper wires | As needed |
| Power supply (5V) | 1 |

---

## Circuit Diagram / Pin Mapping

| DFPlayer Mini | Arduino UNO |
|---|---|
| TX | Pin 9 |
| RX | Pin 10 |

| Sensor | TRIG | ECHO |
|---|---|---|
| Sensor 1 | Pin 2 | Pin 3 |
| Sensor 2 | Pin 4 | Pin 5 |
| Sensor 3 | Pin 6 | Pin 7 |
| Sensor 4 | Pin 8 | Pin 12 |

> **Note:** DFPlayer VCC → 5V, GND → GND, SPK_1/SPK_2 → Speaker.  
> Each HC-SR04 VCC → 5V, GND → GND.

---

## Software Setup

1. Install the **Arduino IDE** (https://www.arduino.cc/en/software)
2. Install the required libraries via Library Manager:
   - `DFRobotDFPlayerMini` by DFRobot
   - `SoftwareSerial` (built-in with Arduino IDE)
3. Copy the code from `src/gesture_tune.ino` into a new Arduino sketch
4. Format a microSD card as **FAT16/FAT32**
5. Copy `.mp3` files onto the card (rename to `0001.mp3`, `0002.mp3`, etc. for reliable ordering)
6. Insert the microSD card into the DFPlayer Mini
7. Select the correct **Board** (Arduino UNO) and **Port**
8. Upload the code

### Pin Configuration (in code)

All pins are defined as macros at the top of the sketch and can be adjusted to match your wiring.

---

## Code

```cpp
#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"

// DFPlayer Mini pins
SoftwareSerial mySerial(9, 10); // TX=9, RX=10
DFRobotDFPlayerMini myDFPlayer;

// Ultrasonic sensor pins
#define trig1 2
#define echo1 3
#define trig2 4
#define echo2 5
#define trig3 6
#define echo3 7
#define trig4 8
#define echo4 12

long duration;
int distance;

// Track play state
bool isPlaying = false;

// Function: Measure distance from Ultrasonic
int getDistance(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH, 20000); // timeout 20ms
  if (duration == 0) return 999; // no object detected
  distance = duration * 0.034 / 2; // cm
  return distance;
}

void setup() {
  mySerial.begin(9600);
  Serial.begin(9600);

  if (!myDFPlayer.begin(mySerial)) {
    Serial.println("DFPlayer Mini not found!");
    while (true);
  }
  myDFPlayer.volume(20); // Set initial volume (0-30)

  // Setup pins
  pinMode(trig1, OUTPUT); pinMode(echo1, INPUT);
  pinMode(trig2, OUTPUT); pinMode(echo2, INPUT);
  pinMode(trig3, OUTPUT); pinMode(echo3, INPUT);
  pinMode(trig4, OUTPUT); pinMode(echo4, INPUT);

  Serial.println("System Ready...");
}

void loop() {
  int d1 = getDistance(trig1, echo1);
  int d2 = getDistance(trig2, echo2);
  int d3 = getDistance(trig3, echo3);
  int d4 = getDistance(trig4, echo4);

  // Debugging
  Serial.print("D1: "); Serial.print(d1);
  Serial.print(" cm | D2: "); Serial.print(d2);
  Serial.print(" cm | D3: "); Serial.print(d3);
  Serial.print(" cm | D4: "); Serial.println(d4);

  // Sensor 1 -> Play/Pause (toggle)
  if(d1 > 0 && d1 <= 10 && isPlaying) {
    myDFPlayer.pause();
    isPlaying = false;
    delay(600);
  }
  else if(d1 >= 11 && d1 <= 20 && !isPlaying) {
    myDFPlayer.play();
    isPlaying = true;
    delay(600);
  }

  // Sensor 2 -> Next/Previous
  if(d2 > 0 && d2 <= 10) {
    myDFPlayer.next();
    delay(600);
  }
  else if(d2 >= 11 && d2 <= 20) {
    myDFPlayer.previous();
    delay(600);
  }

  // Sensor 3 -> Volume Up
  if(d3 > 0 && d3 <= 10) {
    myDFPlayer.volumeUp();
    delay(300);
  }

  // Sensor 4 -> Volume Down
  if(d4 > 0 && d4 <= 10) {
    myDFPlayer.volumeDown();
    delay(300);
  }

  delay(200);
}
```

---

## How It Works

```
HC-SR04 Sensors (4x)         DFPlayer Mini
      │ (distance cm)            │ (I2S audio)
      ▼                          ▼
  Arduino UNO ──SoftwareSerial──► DFPlayer Mini ──► Speaker
      │
      └──► Serial Monitor (debug)
```

1. Each **HC-SR04** emits an ultrasonic pulse and measures the echo return time to calculate distance
2. The **Arduino UNO** reads all four sensors and maps distances to gestures
3. When a valid gesture is detected, the Arduino sends the appropriate command over **SoftwareSerial** to the **DFPlayer Mini**
4. The **DFPlayer Mini** reads MP3 files from the microSD card and outputs audio through the **speaker**
5. All sensor readings are printed to the **Serial Monitor** for real-time debugging (`D1: 5 cm | D2: 15 cm | ...`)

---

## Configuration

All settings are compile-time constants in the sketch:

| Setting | Value |
|---|---|
| Volume range | 0–30 (initial: 20) |
| PulseIn timeout | 20,000 µs (20 ms) |
| Play/Pause debounce | 600 ms |
| Next/Previous debounce | 600 ms |
| Volume debounce | 300 ms |
| Main loop delay | 200 ms |
| No-object return value | 999 |

Adjust these values directly in the `.ino` file to change sensitivity or timing.

---

## Project Structure

```
Touchless_music_controler/
├── Code.docx              # Arduino source code (originally in Word)
├── hci.pptx               # Project presentation (11 slides)
├── h1.jpeg                # Hardware image 1
├── h2.jpeg                # Hardware image 2
├── h3.png                 # Hardware image 3
└── README.md              # This file
```

---

## Hardware Prototype

*(Insert photos from `h1.jpeg`, `h2.jpeg`, `h3.png` here in your GitHub release)*

---

## User Survey Results

A usability survey (presented in `hci.pptx`) reported:

- **53%** found gesture controls intuitive
- **3.93 / 5** average satisfaction rating
- **60%** confirmed the 1–20 cm detection range was sufficient

---

## Future Improvements

- Extend detection range beyond 20 cm
- Add LED feedback for gesture confirmation
- Implement Bluetooth for wireless audio streaming
- Expand gesture complexity (swipe, shake, etc.)
- User-customizable gesture-to-action mapping
- Battery-powered portable version

---

## References

- Norman, D. (2013). *The Design of Everyday Things*
- Arduino Documentation — https://www.arduino.cc/reference
- DFRobot DFPlayer Mini Documentation
- HC-SR04 Ultrasonic Sensor Datasheet

---

## License

This project is developed for academic purposes at **American International University-Bangladesh (AIUB)**.
