require("objects")

class Root < Cota
  def initialize
    super
    @name = Name.new("/")
    @scope = Scope.new

    @scope.add(Primitive.new("int", 4))
  end
end

class Primitive < Cota
  attr :size, true
  def initialize(name, size)
    @name = Name.new(name)
    @scope = Scope.new
    @size = size
  end
end
