def newlineType(input)
  # type = :windows, :unix, :mixed
  type = nil

  buffer = nil
  case input
  when String
    buffer = String.loadBinary(input).unpack("C*")
  when Array
    buffer = input
  else
    raise "ERROR::I take only a filename or byte buffer as input."
  end

  buffer.each_with_index do |byte, ix|
    case byte
    when 0x0a
      if ix > 0 && buffer[ix-1] == 0x0d
        if type.nil? || type == :windows
          type = :windows
        else
          type = :mixed
          break
        end
      else
        if type.nil? || type == :unix
          type = :unix
        else
          type = :mixed
          break
        end
      end
    when 0x0d
      if buffer.length > ix + 1 and buffer[ix + 1] != 0x0a
        puts("WARNING::Found a lonely 0x0d")
        type = :mixed
      end
    end
  end

  return type
end

def changeNewlineType(input, wantedType)
  buffer = nil
  case input
  when String
    buffer = String.loadBinary(input).unpack("C*")
  when Array
    buffer = input
  else
    raise "ERROR::I take only a filename or byte buffer as input."
  end

  ix = 0
  while (ix < buffer.length)
    byte = buffer[ix]

    case byte
    when 0x0a
      # Check if 0x0d is present before 0x0a
      has0d = (ix > 0 && buffer[ix-1] == 0x0d)
      # Take action depending of wantedType
      case wantedType
      when :unix
        if has0d
          buffer.delete_at(ix-1)
          ix -= 1
        end
      when :windows
        if !has0d
          buffer.insert(ix, 0x0d)
          ix += 1
        end
      else
        raise "ERROR::Unknown type #{wantedType}"
      end
      # Proceed
      ix += 1

    when 0x0d
      # Check that there is a 0x0a after 0x0d
      has0a = (buffer.length > ix+1 && buffer[ix+1] == 0x0a)
      if has0a
        # Proceed
        ix += 1
      else
        # We have a lonely 0x0d, remove it
        buffer.delete_at(ix)
      end

    else
      # Proceed
      ix += 1
    end

  end

  case input
  when String
    buffer.pack("C*").export(input)
  end

  nil
end
