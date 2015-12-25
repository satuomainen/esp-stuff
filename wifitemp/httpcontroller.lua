-- httpcontroller.lua
--
-- Handle posting the reading to the server

local CONFIG = require('config')

local httpcontroller = {}

function getConnection()
  local conn = net.createConnection(net.TCP, 0) 
  conn:on("sent", function(conn) conn:close() end)
  return conn
end

function httpcontroller.postData(temperature)
  if temperature == nil then
    return
  end

  for i, serverConfig in ipairs(CONFIG.SERVERS) do 
    postToServer(temperature, serverConfig)
  end
end

function postToServer(temperature, serverConfig)
local readingData = "value=" .. temperature .. "&" .. serverConfig.HTTP_SERVER_API_KEY
  local conn = getConnection()

  conn:connect(serverConfig.HTTP_SERVER_PORT, serverConfig.HTTP_SERVER_IP)
  conn:send("POST " .. serverConfig.HTTP_SERVER_RESOURCE .. " HTTP/1.1\r\n") 
  conn:send("Host: " .. serverConfig.HTTP_SERVER_HOST .. "\r\n") 
  conn:send("Accept: */*\r\n") 
  conn:send("Connection: close\r\n")
  conn:send("Content-Length: " .. string.len(readingData) .. "\r\n")
  conn:send("Content-Type: application/x-www-form-urlencoded\r\n")
  conn:send("User-Agent: ESP8266 (compatible; esp8266 Lua;)\r\n")
  conn:send("\r\n")
  conn:send(readingData)
end

return httpcontroller
