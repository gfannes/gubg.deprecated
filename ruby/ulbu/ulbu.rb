require("tools/allTools")

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

module UlbuParser
  def prepareParsing
  end

  def create4Parser(parent)
    case parent

    when NilClass
      block = Block.new
      block.name = Name.new(@baseName)
      block.body = create(Body.new)
      return block

    when Block
      block = parent
      puts("Buffer for block = (#{buffer[0, 10]})")
      if !matches(/^([@\$\+\-\.]*)/) do |attr|
          block.attributes = attr
        end
        raise "ERROR::Could not read the attributes"
      end
      puts("Buffer for blockk = (#{buffer[0, 10]})")
      if !matches(/^([a-zA-Z]+):/) do |name|
          puts("Block with name \"#{name}\" found")
          block.name = Name.new(name)
        end
        raise "ERROR::Could not read the name"
      end
      if block.attributes[/\$/]
        if !matches("{", "}") do
            block.body = buffer[0, buffer.index("}")]
            consume(block.body.length)
          end
          raise "ERROR::Could not create directive"
        end        
      else
        if matches("{", "}") do
            puts("\twith body")
            block.body = create(Body.new)
          end
        else
          block.body = create(Name.new)
          raise "ERROR::Could not create the body" if block.body.nil?
          puts("\tname reference = #{block.body.name}")
        end
      end
      return block

    when Body
      body = parent
      loop do
        break if buffer[/\}/]
        body.blocks << create(Block.new)
      end
      return body

    when Name
      name = parent
      if !matches(/^([a-zA-Z\.]+)/) do |n|
          name.name = n
        end
        raise "ERROR::Could not create the name"
      end
      return name

    end
    nil
  end

  def finishedParsing
  end
end

parser = Parser.new(UlbuParser)

tree = parser.parse("data/test.ulbu")

