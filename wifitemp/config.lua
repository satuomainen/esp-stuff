-- config.lua
--
-- Read configuration from config.json and return it as an object

local CONFIG = {}

file.open("config.json")
local contents = file.read()
file.close()

if contents ~= nil then
  CONFIG = cjson.decode(contents)
end

return CONFIG
