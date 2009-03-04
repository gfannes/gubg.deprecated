class Name < String
  attr :name, true
  def initialize(name = nil)
    replace(name) if name
  end
end

class Attributes < String
  def initialize(attributes = "")
    replace(attributes)
  end
  def set(attributes)
    replace(attributes)
  end
  def directive?
    self[/\$/]
  end
  def public?
    self[/\+/]
  end
  def volatile?
    self[/\//]
  end
end

class Cota
  attr :name, true
  attr :attributes, true
  attr :refCota, true
  attr :scope, true

  def add(subtree, location)
    return false if location[0] != @name
    case @scope
    when Scope
      location.shift
      @scope.add(subtree, location)
    else
      raise "ERROR::Trying to add to a non-direct location"
    end
    true
  end
  def resolved?
    return !@scope.nil?
  end
  def direct?
    return @refCota.nil?
  end
  def outputSize
    if @scope
      @scope.outputSize
    else
      @refCota.outputSize
    end
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
  def outputSize
    size = 0
    @cotas.each do |cota|
      size += cota.outputSize
    end
    size
  end
end
