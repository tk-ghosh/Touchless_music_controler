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

  // --------------------
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

  // --------------------
  // Sensor 2 -> Next/Previous
  // Close (0-10cm) -> Next, Far (11-20cm) -> Previous
  if(d2 > 0 && d2 <= 10) {
    myDFPlayer.next();
    delay(600);
  }
  else if(d2 >= 11 && d2 <= 20) {
    myDFPlayer.previous();
    delay(600);
  }

  // --------------------
  // Sensor 3 -> Volume Up
  if(d3 > 0 && d3 <= 10) {
    myDFPlayer.volumeUp();
    delay(300);
  }

  // --------------------
  // Sensor 4 -> Volume Down
  if(d4 > 0 && d4 <= 10) {
    myDFPlayer.volumeDown();
    delay(300);
  }

  delay(200);
}
