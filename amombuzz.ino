#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiServer.h>

// Network credentials
const char* ssid = "SpectrumSetup-DD";
const char* password = "jeansrocket543";

// Pin definitions
const int buzzerPin = 12;

WiFiServer server(80);

void setup() {
  Serial.begin(115200);
  pinMode(buzzerPin, OUTPUT);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("Connected to WiFi");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  WiFiClient client = server.available(); // Listen for incoming clients

  if (client) { // If a new client connects,
    Serial.println("New Client."); // print a message out in the serial port
    String currentLine = ""; // make a String to hold incoming data from the client
    while (client.connected()) { // loop while the client's connected
      if (client.available()) { // if there's bytes to read from the client,
        char c = client.read(); // read a byte, then
        Serial.write(c); // print it out the serial monitor
        currentLine += c;
        if (c == '\n') { // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/plain");
            client.println("Connection: close");
            client.println();

            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        }

        // Check to see if the request is for the /buzz URL:
        if (currentLine.endsWith("GET /buzz HTTP/1.1")) {
          playDoorbellJingle();
          currentLine = ""; // Clear the current line after handling the request
        }
      }
    }
    // Give the client time to receive the data
    delay(1);
    // Close the connection
    client.stop();
    Serial.println("Client Disconnected.");
  }
}

// Function to play the doorbell jingle
void playDoorbellJingle() {
  Serial.println("Playing doorbell jingle.");
  // Example melody for "Knockin' on Heaven's Door"
  int melody[] = { 262, 294, 330, 349, 392, 440, 494 };
  int noteDurations[] = { 4, 4, 4, 4, 4, 4, 4 };

  for (int thisNote = 0; thisNote < 7; thisNote++) {
    int noteDuration = 1000 / noteDurations[thisNote];
    tone(buzzerPin, melody[thisNote], noteDuration);
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    noTone(buzzerPin);
  }

  Serial.println("Jingle played.");
}
