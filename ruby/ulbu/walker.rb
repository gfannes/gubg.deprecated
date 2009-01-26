require("objects")
require("tools/asm")

module ResolveWalker
  def prepareWalking
    @scopePerName = {}
    @unresolvedCotas = []
  end

  def walk4Walker(obj)
    case obj
    when Name

    when Cota
      cota = obj
      if cota.resolved?
        puts("Found resolved cota #{cota.name.name}")
        @scopePerName[cota.name.name] = cota.scope
        resolve
        walk4Walker(cota.scope) if cota.direct?
      else
        @unresolvedCotas << cota
      end

    when Scope
      scope = obj
      scope.cotas.each do |cota|
        walk4Walker(cota)
      end

    else

    end
    nil
  end

  def finishWalking
    resolve
    if !@unresolvedCotas.empty?
      @unresolvedCotas.each do |cota|
        puts("Unresolved cota: #{cota.name.name}")
      end
      raise "ERROR::Unresolved cota's still present"
    end
  end

  def resolve
    loop do
      nrResolved = 0
      # Go over the unresolved cota's
      @unresolvedCotas.reject! do |cota|
        # Check if we can resolve this one
        scope = @scopePerName[cota.refName.name]
        if scope
          cota.scope = scope
          @scopePerName[cota.name.name] = scope
          nrResolved += 1
          # Resolved, remove it
          true
        else
          # Not resolved yet
          false
        end
      end
      break if nrResolved == 0
    end
  end
end

module CompileWalker
  def fileName=(fileName)
    @fileName = fileName
  end

  def prepareWalking
    @asm = ASM.new
#   asm.data.add(:ascii, "startMessage", "Hello World\n")
#   asm.bss.addGlobal("_start")
#   start = asm.text.addFunction("_start")
#   start.add([
#               "movl $12, %edx  # Write start message",
#               "movl $startMessage, %ecx",
#               "movl $1, %ebx",
#               "movl $4, %eax",
#               "int $0x80",
#               "movl $0, %ebx   # Exit",
#               "movl $1, %eax",
#               "int $0x80",
#             ])

  end

  def walk4Walker(obj)
    case obj
    when Name
      name = obj

    when Cota
      cota = obj

    when Scope
      scope = obj

    else
      puts("Unknown type #{obj.class}")
    end
  end

  def finishWalking
    @asm.export(@fileName)
  end
end

module PrintWalker
  def prepareWalking
    @indentLevel = 0
  end

  def walk4Walker(obj)
    case obj
    when Name
      name = obj
      puts(" => #{name.name}")

    when Cota
      cota = obj
      print("#{indent}#{cota.name.name} (#{cota.scope})")
      @indentLevel += 1
      if cota.direct?
        walk4Walker(cota.scope)
      else
        puts(" (#{cota.refName.name})")
      end
      @indentLevel -= 1

    when Scope
      scope = obj
      puts(" (#{scope.cotas.length} cotas)")
      scope.cotas.each do |cota|
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
