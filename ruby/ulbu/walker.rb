require("objects")

module ResolveWalker
  def prepareWalking
  end

  def walk4Walker(obj)
    case obj
    when Name

    when Cota

    when Root

    when Scope

    else

    end
  end

  def finishWalking
  end
end

module PrintWalker
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
      walk4Walker(obj.scope)
      @indentLevel -= 1

    when Scope
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
