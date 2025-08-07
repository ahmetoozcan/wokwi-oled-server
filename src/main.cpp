// Display
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Network
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <uri/UriBraces.h>

#define WIFI_SSID "Wokwi-GUEST"
#define WIFI_PASS ""
#define WIFI_CHANNEL 6

WebServer server(8080);

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define OLED_ADDRESS 0x3C

// I2C (GPIO21 = SDA, GPIO22 = SCL)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void handleRoot();
void handlePost();
void handleMessage(String message);
void wifiConnected();

void handleRoot() {
    String website = R"rawliteral(
<!DOCTYPE html>
<html>
  <head>
    <title>ESP32 POST Demo</title>
    <style>
      body {
        font-family: Arial, sans-serif;
        text-align: center;
        background-color: #f5f5f5;
        padding: 2em;
      }
      h1 {
        color: #333;
      }
      form {
        background: #fff;
        padding: 1.5em;
        border-radius: 10px;
        display: inline-block;
        box-shadow: 0 2px 8px rgba(0,0,0,0.2);
      }
      input[type="text"] {
        padding: 0.5em;
        font-size: 1em;
        width: 200px;
        margin-bottom: 1em;
        border: 1px solid #ccc;
        border-radius: 5px;
      }
      input[type="submit"] {
        background-color: #28a745;
        color: white;
        border: none;
        padding: 0.5em 1em;
        font-size: 1em;
        cursor: pointer;
        border-radius: 5px;
      }
      input[type="submit"]:hover {
        background-color: #218838;
      }
    </style>
  </head>
  <body>
    <h1>Send POST Data to ESP32</h1>
    <form action="/submit" method="post">
      <input type="text" name="message" placeholder="Enter message" />
      <br>
      <input type="submit" value="Send" />
    </form>
  </body>
</html>
)rawliteral";


    server.send(200, "text/html", website);
}


void handlePost() {
    if (server.hasArg("message")) {
        String message = server.arg("message");

        Serial.println("Received POST message: " + message);

        handleMessage(message);

        // resend the main page
        handleRoot();
    }
    else {
        handleMessage("");
    }
}

void handleMessage(String message) {
    display.clearDisplay();
    display.setCursor(0, 0);

    message.isEmpty() ? display.println("The message cannot be empty!") : display.println(message);

    display.display();
}

void wifiConnected() {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println("Connected to WiFi");
    display.println(WiFi.localIP());
    display.println("Visit http://localhost:8080 on your browser");
    display.println("Every message you submit will be printed here");
    display.display();
}

void setup() {
    Serial.begin(115200);

    Serial.println(F("Setting up OLED DISPLAY..."));
    while (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS)) {
        delay(1000);
        Serial.println(F("SSD1306 allocation failed..."));
    }

    WiFi.begin(WIFI_SSID, WIFI_PASS, WIFI_CHANNEL);
    Serial.print(F("Connecting to WIFI..."));

    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.print(".");
    }
    // for newline 
    Serial.println("");

    wifiConnected();

    server.on("/", HTTP_GET, handleRoot);
    server.on("/submit", HTTP_POST, handlePost);

    server.begin();
}

void loop() {
    server.handleClient();
}
