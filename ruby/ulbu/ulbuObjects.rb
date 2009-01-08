class Name
  attr :name, true
  def initialize(name = nil)
    @name = name
  end
end

class Block
  attr :name, true
  attr :attributes, true
  attr :body, true
end

class Body
  attr :blocks, true
  def initialize
    @blocks = []
  end
end
