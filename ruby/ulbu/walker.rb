require("objects")

module UlbuWalker
  def prepareWalking
    @indentLevel = 0
  end

  def walk4Walker(obj)
    case obj
    when Name
      puts(" => #{obj.name}")

    when Cota
      print("#{indent}#{obj.name.name}")
      @indentLevel += 1
      walk4Walker(obj.body)
      @indentLevel -= 1

    when Body
      puts(" (#{obj.cotas.length} cotas)")
      obj.cotas.each do |cota|
        walk4Walker(cota)
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
