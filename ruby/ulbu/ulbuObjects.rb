class Name
  attr :name, true
  def initialize(name = nil)
    @name = name
  end
end

class Cota
  attr :name, true
  attr :attributes, true
  attr :body, true
end

class Body
  attr :cotas, true
  def initialize
    @cotas = []
  end
end
