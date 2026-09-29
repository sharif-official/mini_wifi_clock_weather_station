#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <time.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

const char* WIFI_SSID = "YOUR_WIFI_NAME";
const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";

const float LATITUDE = 23.8103;
const float LONGITUDE = 90.4125;

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

Adafruit_SH1106G display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const char* NTP_SERVER = "pool.ntp.org";
const long GMT_OFFSET_SEC = 6 * 3600;
const int DAYLIGHT_OFFSET_SEC = 0;

float temperature = 0.0;
float windSpeed = 0.0;
int weatherCode = 0;
String weatherText = "Loading...";

unsigned long lastWeatherUpdate = 0;
const unsigned long WEATHER_UPDATE_INTERVAL = 10UL * 60UL * 1000UL;

void connectWiFi();
void getWeather();
String getWeatherDescription(int code);
void showClock();

void setup() {
  Serial.begin(115200);
  Wire.begin(21, 22);

  if (!display.begin(0x3C, true)) {
    Serial.println("OLED not found!");
    while (true) delay(1000);
  }

  display.clearDisplay();
  display.setTextColor(SH110X_WHITE);
  display.setTextSize(1);
  display.setCursor(20, 25);
  display.println("Starting...");
  display.display();
  delay(1500);

  connectWiFi();

  configTime(GMT_OFFSET_SEC, DAYLIGHT_OFFSET_SEC, NTP_SERVER);

  struct tm timeinfo;
  while (!getLocalTime(&timeinfo)) {
    Serial.println("Waiting for NTP...");
    delay(1000);
  }

  getWeather();
  lastWeatherUpdate = millis();
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) connectWiFi();

  if (millis() - lastWeatherUpdate > WEATHER_UPDATE_INTERVAL) {
    getWeather();
    lastWeatherUpdate = millis();
  }

  showClock();
  delay(500);
}

void connectWiFi() {
  Serial.print("Connecting to WiFi");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  int counter = 0;
  while (WiFi.status() != WL_CONNECTED && counter < 30) {
    delay(500);
    Serial.print(".");
    counter++;
  }

  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("WiFi Connected!");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("WiFi connection failed!");
  }
}

void getWeather() {
  if (WiFi.status() != WL_CONNECTED) return;

  HTTPClient http;

  String url =
    "https://api.open-meteo.com/v1/forecast?"
    "latitude=" + String(LATITUDE, 4) +
    "&longitude=" + String(LONGITUDE, 4) +
    "&current=temperature_2m,weather_code,wind_speed_10m"
    "&timezone=Asia%2FDhaka";

  http.begin(url);
  int httpCode = http.GET();

  if (httpCode == 200) {
    String payload = http.getString();
    DynamicJsonDocument doc(4096);

    if (!deserializeJson(doc, payload)) {
      temperature = doc["current"]["temperature_2m"];
      weatherCode = doc["current"]["weather_code"];
      windSpeed = doc["current"]["wind_speed_10m"];
      weatherText = getWeatherDescription(weatherCode);
    }
  } else {
    Serial.print("HTTP Error: ");
    Serial.println(httpCode);
  }

  http.end();
}

String getWeatherDescription(int code) {
  if (code == 0) return "Clear Sky";
  if (code == 1 || code == 2 || code == 3) return "Cloudy";
  if (code == 45 || code == 48) return "Fog";
  if (code >= 51 && code <= 67) return "Drizzle/Rain";
  if (code >= 71 && code <= 77) return "Snow";
  if (code >= 80 && code <= 82) return "Rain Showers";
  if (code >= 95 && code <= 99) return "Thunderstorm";
  return "Unknown";
}

void showClock() {
  struct tm timeinfo;

  if (!getLocalTime(&timeinfo)) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(10, 30);
    display.println("Time unavailable");
    display.display();
    return;
  }

  char timeString[10];
  char ampm[4];
  char dateString[20];

  strftime(timeString, sizeof(timeString), "%I:%M:%S", &timeinfo);
  strftime(ampm, sizeof(ampm), "%p", &timeinfo);
  strftime(dateString, sizeof(dateString), "%d %b %Y", &timeinfo);

  display.clearDisplay();

  display.setTextSize(2);
  display.setCursor(4, 2);
  display.print(timeString);

  display.setTextSize(1);
  display.setCursor(105, 8);
  display.print(ampm);

  display.setCursor(29, 22);
  display.print(dateString);

  display.drawLine(0, 33, 127, 33, SH110X_WHITE);

  display.setCursor(0, 39);
  display.print("Temp: ");
  display.print(temperature, 1);
  display.print((char)247);
  display.print("C");

  display.setCursor(0, 51);
  display.print(weatherText);

  display.setCursor(88, 51);
  display.print(windSpeed, 0);
  display.print("km/h");

  display.display();
}
