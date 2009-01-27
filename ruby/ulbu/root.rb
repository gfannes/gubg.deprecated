require("objects")

class Root < Cota
  def initialize
    super
    @name = Name.new("/")
    @scope = Scope.new

    @scope.add(PrimitiveCota.new("int", 4))
  end
end

class PrimitiveCota < Cota
  def initialize(name, size)
    @name = Name.new(name)
    @scope = PrimitiveScope.new(size)
  end
end

class PrimitiveScope < Scope
  attr :outputSize, true
  def initialize(size)
    @outputSize = size
    super()
  end
end
