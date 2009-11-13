gubg = gubg or {}
local _G = _G
setfenv(1, gubg)

local function print(...)
--   _G.print(_G.unpack(arg))
end

function split(str, char)
   local fields = {}
   _G.string.gsub(str, "([^"..char.."]*)", function(field) _G.table.insert(fields, field) end, 1)
   _G.string.gsub(str, char.."([^"..char.."]*)", function(field) _G.table.insert(fields, field) end)
   return fields
end

return gubg
