require "objects"

module UlbuParser
  def prepareParsing
  end

  def create4Parser(parent)
    case parent

    when NilClass
      cota = Cota.new
      cota.name = Name.new(@baseName)
      cota.scope = create(Scope.new)
      return cota

    when Cota
      cota = parent
      # Read the attributes
      cota.attributes = create(Attributes.new)
      # Read the name
      cota.name = create(Name.new)
      if !matches(/\A[ \n]*:/)
        raise "ERROR::Could not read the name"
      end

      if cota.attributes.directive?
        # Read the directive as-is
        if !matches("{", "}") do
            cota.scope = buffer[0, buffer.index("}")]
            consume(cota.scope.length)
          end
          raise "ERROR::Could not create directive"
        end        
      else
        # Read the scope
        if matches("{", "}") do
            cota.scope = create(Scope.new)
          end
        else
          cota.refCota = Cota.new
          cota.refCota.name = create(Name.new)
          raise "ERROR::Could not create the refCota" if cota.refCota.name.nil?
        end
      end
      return cota

    when Scope
      scope = parent
      loop do
        break if (buffer[/\A[ \n]*\}/] or buffer[/\A[ \n]*\z/])
        scope.cotas << create(Cota.new)
      end
      return scope

    when Attributes
      attributes = parent
      if !matches(/\A[ \n]*([@\$\+\-\.]*)/) do |attr|
          attributes.set(attr)
        end
        raise "ERROR::Could not read the attributes"
      end
      return attributes

    when Name
      name = parent
      if !matches(/\A[ \n]*([a-zA-Z\.]+)/) do |n|
          name = n
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
