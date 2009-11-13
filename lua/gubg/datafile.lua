require("gubg.filesystem")
require("gubg.string")

gubg = gubg or {}
local _G = _G
setfenv(1, gubg)

local function print(...)
--   _G.print(_G.unpack(arg))
end

function loadData(fileName, delim)
   local data = {}
   -- Load fileName, line per line
   data.records = loadLines(fileName)
   -- Split of the first line which contains the fieldNames, and split it
   data.fieldNames = split(_G.table.remove(data.records, 1), delim)
   -- Prepare the fields as an array of arrays
   -- Prepare the fieldTypes, at first as an array of array, next, we will converge to 1 type per field
   data.fields = {}
   data.fieldTypes = {}
   for cix, _ in _G.ipairs(data.fieldNames) do
      data.fields[cix] = {}
      data.fieldTypes[cix] = {}
   end
   -- Iterate over the records, split them but keep them in original string format
   -- Add the converted value to the fields
   for rix, record in _G.ipairs(data.records) do
      record = split(record, delim)
      data.records[rix] = record
      for cix, value in _G.ipairs(record) do
	 value = _G.tonumber(value) or value
	 data.fields[cix][rix] = value
	 data.fieldTypes[cix][_G.type(value)] = true
      end
   end
   -- Resolve the data fieldTypes. Depending on them, copy the string format from the records back to the fields, or the converted value from the fields to the records
   for cix, fieldTypes in _G.ipairs(data.fieldTypes) do
      if fieldTypes.string then
	 for rix, record in _G.ipairs(data.records) do
	    record[cix] = data.fields[cix][rix]
	 end
	 data.fieldTypes[cix] = "string"
      elseif fieldTypes.number then
	 for rix, record in _G.ipairs(data.records) do
	    data.fields[cix][rix] = record[cix]
	 end
	 data.fieldTypes[cix] = "number"
      else
	 data.fieldTypes[cix] = "unknown"
      end
   end
   return data
end

return gubg
