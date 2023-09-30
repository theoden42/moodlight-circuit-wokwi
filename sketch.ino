#include <WiFi.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>

// Constants of operation
const int redPin = 18;
const int greenPin = 19;
const int bluePin = 21;
const std::vector<std::string> genres = {"creative", "calming", "focused", "rejuvenating"};
const std::vector<std::vector<int>> color_values = {
  {255, 255, 51}, 
  {173, 216, 230}, 
  {255, 255, 255}, 
  {255, 213, 128}};
const int updateInterval = 2000;  

// Variables in the operation
std::string curr_genre = "default";

bool fetch_genre() {
  HTTPClient http;
  http.begin("https://api.thingspeak.com/channels/2269047/feeds.json?api_key=6KHIGCEJG28UDMVY&results=1");
  int httpCode = http.GET();


  if (httpCode == HTTP_CODE_OK) {
    String payload = http.getString();
    Serial.println("Data from ThingSpeak: " + payload);
    DynamicJsonDocument doc(2048);
    DeserializationError error = deserializeJson(doc, payload);
    if (!error) {
      curr_genre = doc["feeds"][0]["field1"].as<String>().c_str();
      Serial.println(curr_genre.c_str());
      return true;
    } else {
      Serial.print("Failed to parse JSON: ");
      Serial.println(error.c_str());
    }
  } else {
    Serial.println("Failed to fetch data from ThingSpeak");
  }

  return false;
}

void setup() {
  // Setup pin modes
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);

  // Setup baud rate
  Serial.begin(115200);
  Serial.println("Hello from ESP32!");

  // Setup WiFi connectivity
  Serial.print("Connecting to WiFi");
  WiFi.begin("Wokwi-GUEST", ""); 
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }
  Serial.println(" Connected!");
}

void loop() {

  if (WiFi.status() == WL_CONNECTED) {
    if (!fetch_genre()) {
      Serial.println("Unable to Fetch Current Genre");
    } else {
      int ind = -1;
      for (int i = 0; i < genres.size(); i++) {
        if (genres[i] == curr_genre) {
          ind = i;
          break;
        }
      }
      if (ind == -1) {
        Serial.println("Invalid Genre or Default");
      } else {
        int redValue = color_values[ind][0];
        int greenValue = color_values[ind][1];
        int blueValue = color_values[ind][2];
        analogWrite(redPin, redValue);
        analogWrite(greenPin, greenValue);
        analogWrite(bluePin, blueValue);
      }
    }
  }
  delay(updateInterval);
}
