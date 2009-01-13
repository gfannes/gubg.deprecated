require("tools")
require("parser")
require("walker")

fileName = "data/main.ulbu"

tree = time("Parsing #{fileName}", true) do
  parser = Parser.new(UlbuParser)
  parser.parse(fileName)
end

puts("")

time("Printing", true) do
  walker = Walker.new(UlbuWalker)
  walker.walk(tree)
end
