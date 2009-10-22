require "lfs"

gubg = gubg or {}
local _G = _G
setfenv(1, gubg)

local function print(...)
--   _G.print(_G.unpack(arg))
end

-- Stateless iterator for all files in a path
local function iter(state, file)
   print("\nCall to iter with file = ", file)
   local nextFile = nil
   while not nextFile do
      print("state.path = ", state.path)
      local name = state.iter(nil, file)
      print("name = ", name)
      if name then
	 if name ~= "." and name ~= ".." then
	    local newPath = state.path .. "/" .. name
	    local attr, err = _G.lfs.attributes(newPath)
	    if not attr then
	       print("error = ", err)
	    else
	       if attr.mode == "file" then
		  print("\t=> file")
		  nextFile = name
	       elseif attr.mode == "directory" then
		  print("\t=> directory")
		  if state.recursor(name) then
		     local ii = _G.lfs.dir(newPath)
		     prevState = {iter = state.iter, path = state.path, prev = state.prev}
		     state.iter = ii
		     state.path = newPath
		     state.prev = prevState
		  end
	       end
	    end
	 end
      else		--We are finished iterating this directory. Pop it from s and continue with the previous one
	 print("popping iter")
	 if state.prev then
	    state.iter = state.prev.iter
	    state.path = state.prev.path
	    state.prev = state.prev.prev
	 else
	    state.iter = nil
	    state.path = nil
	    state.prev = nil
	    print("We reached the end")
	    break
	 end
      end
   end
   return nextFile, state.path
end

-- Returns an recursive iterator for all files in path
function eachFile(path, ...)
   local i, s, control = _G.lfs.dir(path)
   local recursor = arg.recursor or function (dir)
				       return not _G.string.find(dir, "^%.")
				    end
   local state = {recursor = recursor, iter = i, path = path, prev = nil}
   return iter, state, control
end

function loadLines(fileName)
   local lines = {}
   local file = _G.assert(_G.io.open(fileName))
   for line in file:lines() do
      _G.table.insert(lines, line)
   end
   return lines
end

function fullPath(file, dir)
   return dir .. "/" .. file
end

return gubg
