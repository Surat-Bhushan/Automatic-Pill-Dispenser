#include <Servo.h>
#include <LiquidCrystal.h>

Servo servo;

// Pin assignments
const int servoPin = 6;
const int buttonPin = 9;
const int ledPin = 7;
const int buzzerPin = 8;

int angle = 0;
int angleIncrement = 45;
int newAngle;
int buttonState;
int movementDelay = 15;  // Decreased delay for faster servo movement

unsigned long startTime;
unsigned long previousMillis = 0;  // Stores the last time the timer was updated
const long interval = 1000;  // 1 second interval

// LCD setup
const int rs = A4, en = A5, d4 = A3, d5 = A2, d6 = A1, d7 = A0;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

unsigned long lastPillDispensedTime = 0; // Time when pill was last dispensed

void setup() {
  Serial.begin(9600);

  pinMode(buttonPin, INPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  digitalWrite(ledPin, LOW);
  digitalWrite(buzzerPin, LOW);

  // ✅ Improved Servo Initialization
  Serial.println("Initializing Servo...");
  servo.attach(servoPin);
  delay(500);

  // Move back and forth slightly to ensure activation
  servo.write(90);
  delay(1000);
  servo.write(45);
  delay(1000);
  servo.write(90);
  delay(1000);
  servo.write(0); // Ensure it starts at 0
  delay(1000);
  Serial.println("Servo Ready!");

  // Initialize LCD
  lcd.begin(16, 2);
  delay(2000);  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Pill Dispenser");

  startTime = millis();
}

void loop() {
  unsigned long currentMillis = millis();
  
  // Check if 1 second has passed
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;  // Save the last update time

    unsigned long elapsedSeconds = (millis() - startTime) / 1000;
    int minutes = elapsedSeconds / 60;
    int seconds = elapsedSeconds % 60;

    // Update the timer on LCD every second
    updateLCD(minutes, seconds);

    // Print time to the Serial Monitor every second
    Serial.print("Time: ");
    Serial.print(minutes);
    Serial.print("m ");
    Serial.print(seconds);
    Serial.println("s");

    // Dispense pill at the start of every minute (exactly at 0 seconds)
    if (elapsedSeconds - lastPillDispensedTime >= 60 && seconds == 0) {
      lastPillDispensedTime = elapsedSeconds;  // Update last dispensed time
      dispensePill();
    }
  }
  
  buttonState = digitalRead(buttonPin);
}

void dispensePill() {
  newAngle = angle + angleIncrement;

  if (newAngle <= 180) {
    while (angle < newAngle) {
      angle += 1;
      servo.write(angle);
      delay(movementDelay);  // Faster movement
    }
  } else {
    //reset back to 0 degrees
    while (angle > 0) {
      angle -= 1;
      servo.write(angle);
      delay(movementDelay);  
    }
  }

  Serial.print("Motor angle is now: ");
  Serial.println(angle);

  triggerAlarm();
}

void triggerAlarm() {
  if (angle == 0) {
    // Display message when lcd screen starts
    lcd.setCursor(0, 0);
    lcd.print("Pill Dispenser is Ready!  ");
    Serial.println("Pill Dispenser is Ready!");

    digitalWrite(ledPin, LOW);
    digitalWrite(buzzerPin, LOW);
  } else {
    // Display message at the time of pill taking
    lcd.setCursor(0, 0);
    lcd.print("Take Medicine! ");
    Serial.println("Take Medicine!");

    digitalWrite(ledPin, HIGH);
    digitalWrite(buzzerPin, HIGH);

    unsigned long startAlarmTime = millis();
    while (digitalRead(buttonPin) == LOW && (millis() - startAlarmTime) < 60000) {
      unsigned long elapsedSeconds = (millis() - startTime) / 1000;
      int minutes = elapsedSeconds / 60;
      int seconds = elapsedSeconds % 60;
      updateLCD(minutes, seconds);
      delay(100);
    }

    digitalWrite(ledPin, LOW);
    digitalWrite(buzzerPin, LOW);
  }

  lcd.setCursor(0, 0);
  lcd.print("Pill Dispenser ");
  Serial.println("Pill Dispenser");
}

void updateLCD(int minutes, int seconds) {
  lcd.setCursor(0, 1);
  lcd.print("Time: ");
  lcd.print(minutes);
  lcd.print("m ");
  lcd.print(seconds);
  lcd.print("s  "); // Extra spaces to clear old characters
}

