#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <Wire.h>
#include <Adafruit_BMP085.h>
#include <DHT.h>

#include "./secrets.h"

// We have a DHT22 connected to Wemos D1 pin D6
#define DHTPIN D6
#define DHTTYPE DHT22   // DHT 22 (AM2302)
DHT dht(DHTPIN, DHTTYPE);

// We have a BMP180 connected to D1
Adafruit_BMP085 bmp;

// Buffer for server url
char url[256];

// Buffer for data that is sent to the server
char postData[256];

// Number of retries reading the DHT22 humidity sensor before giving up
int MAX_DHT_RETRIES = 10;

// The operation indicator LED is controlled by pin D5
#define LED_PIN D5

// Measurement cycle length is 5 minutes (300 sec).
// LED blinks for 5 seconds, restart with Wifi connect is about 2-3 seconds
// => Delay before restart = 300 - 5 - 3 = 292
const long READING_INTERVAL_SECONDS = 292 * 1000;

/* === INITIALIZATION === */

void setup() {
  Serial.begin(115200);
  Serial.print("\n\n\n");

  setupWifi();
  setupBmp180();
  setupDht22();

  setupLed();
}

void setupWifi() {
  WiFi.begin(STASSID, STAPSK);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.printf("\nConnected to %s\n", STASSID);
}

void setupBmp180() {
  if (!bmp.begin()) {
    Serial.println("Could not find BMP180 sensor at 0x77");
  }
}

void setupDht22() {
  pinMode(DHTPIN, INPUT);
  dht.begin();  
}

void setupLed() {
  pinMode(LED_PIN, OUTPUT);
  ledOn();
}

/* === OPERATION === */

void loop() {
  
  // Only try to post if wifi is connected
  if ((WiFi.status() == WL_CONNECTED)) {
    printBmp();
    printDht();
  }

  ledOff();
  delay(READING_INTERVAL_SECONDS);
  blinkLed(5);
  ESP.restart();
}

void ledOn() {
  digitalWrite(LED_PIN, LOW);
}

void ledOff() {
  digitalWrite(LED_PIN, HIGH);
}

void blinkLed(int times) {
  ledOn();
  for (int i = 0; i < times; i++) {
    ledOff();
    delay(500);
    ledOn();
    delay(500);
  }
}

void printDht() {
  int retry = 0;

  while (retry < MAX_DHT_RETRIES) {
    float humidity = dht.readHumidity();
    float temperature = dht.readTemperature();

    // Check if any reads failed and exit early (to try again).
    if (isnan(humidity) || isnan(temperature)) {
      Serial.println("Failed to read from DHT sensor!");
      retry++;
      delay(100);
      continue;
    }

    char temperatureCelcius[32];
    sprintf(temperatureCelcius, "%.2f", temperature);
    sprintf(postData, READING_DATA_TEMPLATE, temperatureCelcius);
    sprintf(url, ADD_READING_URL_TEMPLATE, DHT22_TEMPERATURE_DATASERIES);
    postReading(url, postData);

    char humidityPercent[32];
    sprintf(humidityPercent, "%.2f", humidity);
    sprintf(postData, READING_DATA_TEMPLATE, humidityPercent);
    sprintf(url, ADD_READING_URL_TEMPLATE, DHT22_HUMIDITY_DATASERIES);
    postReading(url, postData);
    
    return;
  }
}

void printBmp() {
  char temperatureCelcius[32];
  sprintf(temperatureCelcius, "%.2f", bmp.readTemperature());
  sprintf(postData, READING_DATA_TEMPLATE, temperatureCelcius);
  sprintf(url, ADD_READING_URL_TEMPLATE, BMP180_TEMPERATURE_DATASERIES);
  postReading(url, postData);

  char pressurehPa[32];
  sprintf(pressurehPa, "%.2f", bmp.readPressure() / 100.0);
  sprintf(postData, READING_DATA_TEMPLATE, pressurehPa);
  sprintf(url, ADD_READING_URL_TEMPLATE, BMP180_PRESSURE_DATASERIES);
  postReading(url, postData);
}

void postReading(char* url, char* data) {
  WiFiClient client;
  HTTPClient http;

  http.begin(client, url);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  int httpCode = http.POST(data);

  if (httpCode > 0) {
    Serial.printf("Posted to %s, DATA: %s\n", url, data);
  } else {
    Serial.printf("Posting readings failed, error: %s\n", http.errorToString(httpCode).c_str());
  }

  http.end();
}
