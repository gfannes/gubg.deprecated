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

  def add(subtree, location)
    return false if location[0] != @name.name
    case @scope
    when Scope
      location.shift
      @scope.add(subtree, location)
    else
      raise "ERROR::Trying to add to a non-direct location"
    end
    true
  end
  def size
  end
end

class Scope
  attr :cotas, true
  def initialize
    @cotas = []
  end

  def add(subtree, location = [])
    if location.empty?
      @cotas << subtree
    else
      @cotas.each do |cota|
        break if cota.add(subtree, location)
      end
    end
  end
end
