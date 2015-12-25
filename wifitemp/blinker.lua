-- Blink the LED connected to GPIO2

local GPIO2_INDEX = 4;
local BLINK_TIMER_ID = 3
local ledStatus = true

local blinker = {}

function blinker.startBlinking()
  gpio.write(GPIO2_INDEX, gpio.LOW)
  tmr.alarm(BLINK_TIMER_ID, 300, 1, toggleLedStatus)
end

function blinker.stopBlinking()
  tmr.stop(BLINK_TIMER_ID)
  gpio.write(GPIO2_INDEX, gpio.HIGH)
end

function toggleLedStatus()
  local level = gpio.LOW;
  if (ledStatus) then
    level = gpio.LOW
  else
    level = gpio.HIGH
  end
  ledStatus = not ledStatus
  gpio.write(GPIO2_INDEX, level)
end

return blinker