-- measurement.lua
--
-- Remote temperature monitoring with ESP8266 with DS18B20 Dallas 1-wire Thermometer
-- 
-- The application's main control logic

local CONFIG = require('config')
local sensor = require('sensor')
local httpcontroller = require('httpcontroller')
local blinker = require('blinker')
local MAX_RETRIES = 10

function saveReading()
  local temperature = nil
  local retryCount = 0
  local shouldRetry = (temperature == nil) and (retryCount < MAX_RETRIES)

  blinker.startBlinking()

  while shouldRetry do
    temperature = sensor.getReading()
    retryCount = retryCount + 1
    shouldRetry = (temperature == nil) and (retryCount < MAX_RETRIES)
  end

  blinker.stopBlinking()

  if temperature ~= nil then
    httpcontroller.postData(temperature)
  end
end

-- Send the measurement once to the server in 5 sec (allow a small delay
-- to be able to say =file.remove("init.lua") in case there is a busy loop)
tmr.alarm(0, 5000, 0, saveReading)
