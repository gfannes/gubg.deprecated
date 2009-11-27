require("lfs")

gubg = gubg or {}
local _G = _G
setfenv(1, gubg)

local function print(...)
   --   _G.print(_G.unpack(arg))
end

local function serializeObject(obj)
   print("Serializing an object of type " .. _G.type(obj) .. "")
   if _G.type(obj) == "number" then
      _G.io.write(obj)
   elseif _G.type(obj) == "string" then
      _G.io.write(_G.string.format("%q", obj))
   elseif _G.type(obj) == "boolean" then
      _G.io.write((obj and "true" or "false"))
   elseif _G.type(obj) == "table" then
      _G.io.write("{\n")
      for k, v in _G.pairs(obj) do
	 _G.io.write("  [")
	 serializeObject(k)
	 _G.io.write("] = ")
	 serializeObject(v)
	 _G.io.write(",\n")
      end
      _G.io.write("}\n")
   else
      _G.error("I cannot serialize a " .. _G.type(obj) .. " object")
   end
end

function serialize(o, ...)
   local file
   local prevOutput
   if (arg[1]) then
      file = _G.assert(_G.io.open(arg[1], "wb"))
      prevOutput = _G.io.output()
      _G.io.output(file)
   end

   _G.io.write("Entry(")
   serializeObject(o)
   _G.io.write(")")

   if (file) then
      file:close()
      _G.io.output(prevOutput)
   end
end

function loadObjects(fileName, func)
   local obj
   local info = _G.lfs.attributes(fileName)
   if (info and info.mode == "file") then
      local tmpEntry = _G.Entry
      _G.Entry = func
      obj = _G.dofile(fileName)
      _G.Entry = tmpEntry
   end
   return obj
end

return gubg
