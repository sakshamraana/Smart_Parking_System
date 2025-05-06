
#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>
#include <Servo.h>

// Wi-Fi credentials
#define WIFI_SSID "sakshamrana"
#define WIFI_PASSWORD "mujhe bhi"

// Firebase credentials
#define FIREBASE_HOST "smart-parking-system-55814-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "NUyNdy6aQLaXfj8X1l7Y5ct01gDoriylXz7Fqs2x" // Or Web API key

// Firebase objects
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

Servo gateServo;

void setup() {
  Serial.begin(115200);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500); Serial.print(".");
  }
  Serial.println("\nWiFi connected.");

  // Firebase config
  config.host = FIREBASE_HOST;
  config.signer.tokens.legacy_token = FIREBASE_AUTH;

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  // Initialize Servo
  gateServo.attach(D4); // GPIO2
  gateServo.write(0);   // Closed position
}

void loop() {
  if (Firebase.getString(fbdo, "/gate_control/payment_status")) {
    String status = fbdo.stringData();
    Serial.println("Payment Status: " + status);

    if (status == "paid") {
      Serial.println("Payment received. Opening gate...");
      gateServo.write(90); // Open
      delay(5000);
      gateServo.write(0);  // Close

      // Reset status
      Firebase.setString(fbdo, "/gate_control/payment_status", "unpaid");
      Serial.println("Status reset.");
    }
  } else {
    Serial.println("Firebase read failed: " + fbdo.errorReason());
  }

  delay(2000); // Wait before checking again
}

