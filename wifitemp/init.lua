-- init.lua
--
-- Remote temperature monitoring with ESP8266 with DS18B20 Dallas 1-wire Thermometer
-- 
-- Configure everything in config.json before starting, the CONFIG object needs
-- to provide configuration parameters across the program.
-- 
-- The application runs on nodemcu (dev096 branch). The nodemcu-library file
-- nodemcu-firmware/lua_modules/ds18b20/ds18b20.lua needs to be uploaded to the
-- module. (git clone https://github.com/nodemcu/nodemcu-firmware.git)

local CONFIG = require('config')
local GPIO2_INDEX = 4;
local blinker = require('blinker')

function wifiReady()
  blinker.stopBlinking()
  gpio.write(GPIO2_INDEX, gpio.HIGH)
  wifi.sta.eventMonStop()
  wifi.sta.eventMonReg(wifi.STA_GOTIP, "unreg")
  dofile("measurement.lua")
end

gpio.mode(GPIO2_INDEX, gpio.OUTPUT)
gpio.write(GPIO2_INDEX, gpio.LOW)

-- Wait a couple of seconds before starting to connect to WLAN
tmr.alarm(6, 2000, 0, function()
  blinker.startBlinking()
  wifi.setmode(wifi.STATION)
  wifi.sta.eventMonReg(wifi.STA_GOTIP, wifiReady)
  wifi.sta.eventMonStart()
  wifi.sta.config(CONFIG.WIFI_AP_NAME, CONFIG.WIFI_PASSWORD, 1)
  wifi.sta.connect()
end)

-- Sleep for the configured time span and start over
tmr.alarm(1, CONFIG.SECONDS_BETWEEN_READINGS * 1000, 1, node.restart)
