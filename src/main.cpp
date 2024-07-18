//Discover logic - periodically the Ip of the device is sent to broadcast and received and displayed
#include <Arduino.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include <vector>
#include <string>

#define SCREEN_WIDTH 64
#define SCREEN_HEIGHT 128

const char* ssid = "FiFi"; // your-SSID
const char* password = "elitebook"; // your-PASSWORD
const int udpPort = 12345;

Adafruit_SH1107 display = Adafruit_SH1107(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
WiFiUDP udp;
IPAddress broadcastIP;

std::vector<std::string> tableOfIPs;//lets change to set (CHat proposed possibly good idea)

void setup() {
  Serial.begin(115200);
  Wire.begin(26, 32);

  if (!display.begin(0x3C, true)) {
    Serial.println("SSD1306 allocation failed");
    for (;;);
  }

  display.setRotation(3);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(0,0);

  display.println("Connecting to WiFi...");
  display.display();

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("Connected to WiFi");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Calculate the broadcast IP address
  IPAddress ip = WiFi.localIP();
  IPAddress subnet = WiFi.subnetMask();
  broadcastIP = ~subnet | ip;

  udp.begin(udpPort);
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("My IP:");
  display.println(WiFi.localIP());
  display.display();
}

void loop() {
  // Broadcast own IP
  String ipString = WiFi.localIP().toString();
  udp.beginPacket(broadcastIP, udpPort);
  udp.write((const uint8_t*)ipString.c_str(), ipString.length());
  udp.endPacket();

  // Listen for incoming UDP packets
  int packetSize = udp.parsePacket();
  if (packetSize) {
    char incomingPacket[255];
    int len = udp.read(incomingPacket, 255);
    if (len > 0) {
      incomingPacket[len] = 0;
    }
    String incomingIP = String(incomingPacket);

    // Check if the IP is already in the table
    bool ipExists = false;
    for (const auto& ip : tableOfIPs) {
      if (ip == incomingIP.c_str()) {
        ipExists = true;
        break;
      }
    }

    // If the IP is not in the table, add it
    if (!ipExists) {
      tableOfIPs.push_back(incomingIP.c_str());
      
      // Display the discovered IPs on the screen
      display.clearDisplay();
      display.setCursor(0, 0);
      display.println("My IP:");
      display.println(WiFi.localIP());
      display.println("Discovered IPs:");
      for (const auto& ip : tableOfIPs) {
        display.println(ip.c_str());
      }
      
      display.display();
    }
  }

  delay(1000);
}

/*
//the same, but stores IPs in teh long string
#include <Arduino.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

//table of strings to hoard the IP's
#include <vector>
#include <string>

#define SCREEN_WIDTH 64
#define SCREEN_HEIGHT 128

const char* ssid = "FiFi";//your-SSID
const char* password = "elitebook";//your-PASSWORD
const int udpPort = 12345;

Adafruit_SH1107 display = Adafruit_SH1107(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
WiFiUDP udp;
IPAddress broadcastIP;
String discoveredIPs = "";

std::vector<std::string> tableOfIPs;

void setup() {
  Serial.begin(115200);
  Wire.begin(26, 32);

  if(!display.begin(0x3C, true)) {
    Serial.println("SSD1306 allocation failed");
    for(;;);
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);  
  display.setCursor(10, 10);   

  display.println("Connecting to WiFi...");
  display.display();

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("Connected to WiFi");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Calculate the broadcast IP address
  IPAddress ip = WiFi.localIP();
  IPAddress subnet = WiFi.subnetMask();
  broadcastIP = ~subnet | ip;

  udp.begin(udpPort);
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("My IP:");
  display.println(WiFi.localIP());
  display.display();
}

void loop() {
  // Broadcast own IP
  String ipString = WiFi.localIP().toString();
  udp.beginPacket(broadcastIP, udpPort);
  udp.write((const uint8_t*)ipString.c_str(), ipString.length());
  udp.endPacket();
  
  // Listen for incoming UDP packets
  int packetSize = udp.parsePacket();
  if (packetSize) {
    char incomingPacket[255];
    int len = udp.read(incomingPacket, 255);
    if (len > 0) {
      incomingPacket[len] = 0;
    }
    String incomingIP = String(incomingPacket);
    if (discoveredIPs.indexOf(incomingIP) == -1) {
      discoveredIPs += incomingIP + "\n";
      display.clearDisplay();
      display.setCursor(0, 0);
      display.println("My IP:");
      display.println(WiFi.localIP());
      display.println("Discovered IPs:");
      display.println(discoveredIPs);
      display.display();
    }
  }

  delay(1000);
}
*/