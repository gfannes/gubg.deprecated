require "objects"

module UlbuParser
  def prepareParsing
  end

  def create4Parser(parent)
    case parent

    when NilClass
      cota = Cota.new
      cota.name = Name.new(@baseName)
      cota.body = create(Body.new)
      return cota

    when Cota
      cota = parent
      if !matches(/\A([@\$\+\-\.]*)/) do |attr|
          cota.attributes = attr
        end
        raise "ERROR::Could not read the attributes"
      end
      if !matches(/\A([a-zA-Z]+):/) do |name|
          puts("Cota with name \"#{name}\" found")
          cota.name = Name.new(name)
        end
        raise "ERROR::Could not read the name"
      end
      if cota.attributes[/\$/]
        if !matches("{", "}") do
            cota.body = buffer[0, buffer.index("}")]
            consume(cota.body.length)
          end
          raise "ERROR::Could not create directive"
        end        
      else
        if matches("{", "}") do
            puts("\twith body")
            cota.body = create(Body.new)
          end
        else
          cota.body = create(Name.new)
          raise "ERROR::Could not create the body" if cota.body.nil?
          puts("\tname reference = #{cota.body.name}")
        end
      end
      return cota

    when Body
      body = parent
      loop do
        break if buffer[/\A[ \n]*\}/] or buffer[/\A[ \n]*$/]
        body.cotas << create(Cota.new)
      end
      return body

    when Name
      name = parent
      if !matches(/\A([a-zA-Z\.]+)/) do |n|
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
