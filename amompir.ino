#include <WiFi.h>
#include <WiFiClient.h>

// Network credentials
const char* ssid = "SpectrumSetup-DD";
const char* password = "jeansrocket543";

// Pin definitions
const int pirPin = 27;
const int ledPin = 2;  // On-board LED

// Timer variables
unsigned long now = millis();
unsigned long lastTrigger = 0;
boolean startTimer = false;
boolean motion = false;

// WiFiClient to maintain the connection
WiFiClient client;

void IRAM_ATTR detectsMovement() {
  digitalWrite(ledPin, HIGH);
  startTimer = true;
  lastTrigger = millis();
}

void setup() {
  // Serial port for debugging purposes
  Serial.begin(115200);
  
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // PIR Motion Sensor mode INPUT_PULLUP
  pinMode(pirPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(pirPin), detectsMovement, RISING);

  // Set LED to LOW
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  // Connect to the buzzer server
  if (!client.connect("192.168.1.54", 80)) { // Replace with the IP address of the buzzer ESP32
    Serial.println("Failed to connect to buzzer unit.");
  }
}

void loop() {
  // Current time
  now = millis();
  
  if ((digitalRead(ledPin) == HIGH) && (motion == false)) {
    Serial.println("MOTION DETECTED!!!");
    motion = true;
    sendNotification();
  }
  
  // Turn off the LED after 5 seconds of no motion
  if (startTimer && (now - lastTrigger > 5000)) {
    Serial.println("Motion stopped...");
    digitalWrite(ledPin, LOW);
    startTimer = false;
    motion = false;
  }
}

void sendNotification() {
  if (client.connected()) {
    client.print("GET /buzz HTTP/1.1\r\n");
    client.print("Host: 192.168.1.54\r\n"); // Replace with the IP address of the buzzer ESP32
    client.print("Connection: keep-alive\r\n");
    client.print("\r\n");
    Serial.println("Notification sent to buzzer unit.");
  } else {
    Serial.println("Connection to buzzer unit lost. Reconnecting...");
    if (client.connect("192.168.1.54", 80)) { // Replace with the IP address of the buzzer ESP32
      sendNotification();
    } else {
      Serial.println("Reconnection failed.");
    }
  }
}
