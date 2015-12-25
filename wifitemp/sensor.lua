-- sensor.lua
--
-- Provide readings from the DS18B20 sensor

local sensor = {}

require('ds18b20')

DS18B20_RESET_VALUE = 85
GPIO0_IO_INDEX      = 3

ds18b20.setup(GPIO0_IO_INDEX)

function sensor.getReading()
  local MAX_RETRIES = 50
  local temperature = ds18b20.read()
  local retry = 0;
  while retry < MAX_RETRIES do 
    retry = retry + 1
    temperature = ds18b20.read()
    isNotNil = temperature ~= nil
    isNotResetValue = temperature ~= DS18B20_RESET_VALUE
    if isNotNil and isNotResetValue then
      return temperature
    end
  end
  return nil
end

return sensor
