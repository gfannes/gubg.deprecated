require("ulbuObjects")

module UlbuWalker
  def prepareWalking
    @indentLevel = 0
  end

  def walk4Walker(obj)
    case obj
    when Name

    when Block
      puts("#{indent}#{obj.name.name}")
      @indentLevel += 1
      walk4Walker(obj.body)
      @indentLevel -= 1

    when Body
      puts("Body has #{obj.blocks.length} blocks")
      obj.blocks.each do |block|
        walk4Walker(block)
      end

    else
      puts("Unknown type #{obj.class}")
    end
  end

  def finishWalking
  end

  def indent
    " "*2*@indentLevel
  end
end
