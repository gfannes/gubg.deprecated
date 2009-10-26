require "lfs"

gubg = gubg or {}
local _G = _G
setfenv(1, gubg)

local function print(...)
--   _G.print(_G.unpack(arg))
end

-- Stateless iterator for all files in a path
local function iteratorFiles(state, file)
   print("\nCall to iteratorFiles with file = ", file)
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

-- Stateless iterator for all dirs in a path
local function iteratorDirs(state, dir)
   print("\nCall to iteratorDirs with dir = ", dir)
   local nextDir = nil
   while not nextDir do
      print("state.path = ", state.path)
      local name = state.iter(nil, dir)
      print("name = ", name)
      if name then
	 if name ~= "." and name ~= ".." then
	    local newPath = state.path .. "/" .. name
	    local attr, err = _G.lfs.attributes(newPath)
	    if not attr then
	       print("error = ", err)
	    else
	       if attr.mode == "directory" then
		  print("\t=> directory")
		  nextDir = name
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
   return nextDir
end

-- Returns an recursive iterator for all files in path
--  * arg[1](dir) can specify if dir should be visited recursively
function eachFile(path, ...)
   local i, s, control = _G.lfs.dir(path)
   local recursor = arg[1] or function (dir)
				 return not _G.string.find(dir, "^%.")
			      end
   local state = {recursor = recursor, iter = i, path = path, prev = nil}
   return iteratorFiles, state, control
end

-- Returns a recursive iterator for all directories in path
--  * arg[1](dir) can specify if dir should be visited recursively
function eachDir(path, ...)
   local i, s, control = _G.lfs.dir(path)
   local recursor = arg[1] or function (dir)
				 return not _G.string.find(dir, "^%.")
			      end
   local state = {recursor = recursor, iter = i, path = path, prev = nil}
   return iteratorDirs, state, control
end

-- Loads fileName, splits it into lines and returns those as an array
--  * arg[1](line) can modify the line that is returned
function loadLines(fileName, ...)
   local lines = {}
   local file = _G.assert(_G.io.open(fileName))
   local modifier = arg[1]
   if modifier then
      for line in file:lines() do
	 local l = modifier(line)
	 if l then _G.table.insert(lines, l) end
      end
   else
      for line in file:lines() do
	 _G.table.insert(lines, line)
      end
   end
   file:close()
   return lines
end

function fullPath(file, dir)
   return dir .. "/" .. file
end

return gubg
