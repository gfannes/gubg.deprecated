require("tools")

def buildExecutable(sourceFile)
  executable = nil

  time("Building executable #{sourceFile}") do
    language = nil
    begin

      # Create the language dependency parser and builder
      case sourceFile
      when /\.d$/
        require("gb/gb-d.rb")
        language = DLanguage.new("base.rb")
      when /\.c$/
      when /\.cpp$/
      else
        puts("ERROR::Could not determine the programming language from \"#{sourceFile}\"")
      end

      # Collect dependency information
      if language
        dependencyInformation = language.collectDependencyInformation(sourceFile)
      else
        puts("ERROR::Invalid language specification")
      end

      # Compile dependencies
      if dependencyInformation
        objectFiles = language.compile(dependencyInformation)
      else
        puts("ERROR::Invalid dependency information")
      end

      # Build the executable
      if objectFiles
        executable = language.build(sourceFile, objectFiles)
      else
        puts("ERROR::Compilation failed")
      end
    ensure
      # Clean the language parser, e.g., removing temporary files
      language.clean if language
    end

    puts("ERROR::Building executable failed") if !executable
  end

  return executable
end
