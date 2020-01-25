#ifndef __SECRETS_H__
#define __SECRETS_H__

// Wifi secrets
const char STASSID[] = "YOUR_AP_NAME";
const char STAPSK[]  = "YOUR_PASSWORD";

// Server configuration - edit these to match your backend setup
const int BMP180_PRESSURE_DATASERIES = 2;
const int BMP180_TEMPERATURE_DATASERIES = 1;
const int DHT22_HUMIDITY_DATASERIES = 4;
const int DHT22_TEMPERATURE_DATASERIES = 3;
const char ADD_READING_URL_TEMPLATE[] = "http://YOURSERVER/series/%d/add";
const char READING_DATA_TEMPLATE[] = "value=%s&api_key=YOUR_SECRET_API_KEY";

#endif
