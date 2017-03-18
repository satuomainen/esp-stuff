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

  conn:dns(serverConfig.HTTP_SERVER_HOST, function(cnn, ipAddress)
      if ipAddress ~= nil then
        cnn:connect(serverConfig.HTTP_SERVER_PORT, ipAddress)
        cnn:send("POST " .. serverConfig.HTTP_SERVER_RESOURCE .. " HTTP/1.1\r\n")
        cnn:send("Host: " .. serverConfig.HTTP_SERVER_HOST .. "\r\n")
        cnn:send("Accept: */*\r\n")
        cnn:send("Connection: close\r\n")
        cnn:send("Content-Length: " .. string.len(readingData) .. "\r\n")
        cnn:send("Content-Type: application/x-www-form-urlencoded\r\n")
        cnn:send("User-Agent: ESP8266 (compatible; esp8266 Lua;)\r\n")
        cnn:send("\r\n")
        cnn:send(readingData)
      end
    end)
end

return httpcontroller
