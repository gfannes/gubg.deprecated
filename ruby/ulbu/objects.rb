class Name
  attr :name, true
  def initialize(name = nil)
    @name = name
  end
end

class Attributes
  attr :attributes, true
  def initialize(attributes = "")
    @attributes = attributes
  end
  def directive?
    @attributes[/\$/]
  end
end

class Cota
  attr :name, true
  attr :attributes, true
  attr :scope, true
end

class Scope
  attr :cotas, true
  def initialize
    @cotas = []
  end
end
