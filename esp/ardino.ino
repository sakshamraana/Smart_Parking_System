#include <Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define IR_SENSOR_1_PIN 2
#define IR_SENSOR_2_PIN 3
#define SERVO_1_PIN 9
#define SERVO_2_PIN 10

const int maxSlots = 6;
int slotCount = 0;

Servo servo1, servo2;
LiquidCrystal_I2C lcd(0x27, 16, 2);

int lastEntryState = HIGH;
int lastExitState = HIGH;

void setup() {
  pinMode(IR_SENSOR_1_PIN, INPUT);
  pinMode(IR_SENSOR_2_PIN, INPUT);

  servo1.attach(SERVO_1_PIN);
  servo2.attach(SERVO_2_PIN);
  servo1.write(0);
  servo2.write(0);

  Serial.begin(9600); // Communication with ESP
  lcd.init();
  lcd.backlight();
  lcd.print("Parking System");
  delay(2000);
  lcd.clear();
}

void loop() {
  int entrySensor = digitalRead(IR_SENSOR_1_PIN);
  int exitSensor = digitalRead(IR_SENSOR_2_PIN);

  // ENTRY
  if (entrySensor == LOW && lastEntryState == HIGH) {
    if (slotCount < maxSlots) {
      slotCount++;
      lcd.clear();
      lcd.print("Car Entering...");
      servo1.write(90);
      delay(2000);
      servo1.write(0);
    } else {
      lcd.clear();
      lcd.print("Parking Full!");
      delay(2000);
    }
  }
  lastEntryState = entrySensor;

  // EXIT (with ESP check)
  if (exitSensor == LOW && lastExitState == HIGH) {
    if (slotCount > 0) {
      lcd.clear();
      lcd.print("Waiting Payment");

      // Ask ESP for payment status
      Serial.println("CHECK");

      unsigned long startTime = millis();
      while (!Serial.available() && millis() - startTime < 5000); // wait max 5 sec

      if (Serial.available()) {
        char response = Serial.read();
        if (response == 'Y') {
          slotCount--;
          lcd.clear();
          lcd.print("Payment OK");
          servo2.write(90);
          delay(2000);
          servo2.write(0);
        } else {
          lcd.clear();
          lcd.print("Payment Failed");
        }
      } else {
        lcd.clear();
        lcd.print("No Response");
      }
    } else {
      lcd.clear();
      lcd.print("No Cars Inside");
    }
    delay(2000);
  }
  lastExitState = exitSensor;

  // Show available slots
  lcd.setCursor(0, 1);
  lcd.print("Slots: ");
  lcd.print(maxSlots - slotCount);
  lcd.print("      ");
  delay(200);
}
                       
