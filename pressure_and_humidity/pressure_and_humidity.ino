#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <Wire.h>
#include <Adafruit_BMP085.h>
#include <DHT.h>

#include "./secrets.h"

#define USE_SERIAL Serial

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

const long READING_INTERVAL_SECONDS = 300 * 1000;

void setup() {
  USE_SERIAL.begin(115200);
  USE_SERIAL.print("\n\n\n");

  setupWifi();
  setupBmp180();
  setupDht22();
}

void setupWifi() {
  WiFi.begin(STASSID, STAPSK);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    USE_SERIAL.print(".");
  }
  USE_SERIAL.printf("\nConnected to %s\n", STASSID);
}

void setupBmp180() {
  if (!bmp.begin()) {
    USE_SERIAL.println("Could not find BMP180 sensor at 0x77");
  }
}

void setupDht22() {
  pinMode(DHTPIN, INPUT);
  dht.begin();  
}

void loop() {
  
  // Only try to post if wifi is connected
  if ((WiFi.status() == WL_CONNECTED)) {
    printBmp();
    printDht();
  }

  delay(READING_INTERVAL_SECONDS);
}

int MAX_DHT_RETRIES = 10;

void printDht() {
  int retry = 0;

  while (retry < MAX_DHT_RETRIES) {
    float humidity = dht.readHumidity();
    float temperature = dht.readTemperature();

    // Check if any reads failed and exit early (to try again).
    if (isnan(humidity) || isnan(temperature)) {
      USE_SERIAL.println("Failed to read from DHT sensor!");
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
    USE_SERIAL.printf("Posted to %s, DATA: %s\n", url, data);
  } else {
    USE_SERIAL.printf("Posting readings failed, error: %s\n", http.errorToString(httpCode).c_str());
  }

  http.end();
}
