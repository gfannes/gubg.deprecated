class Processor
    def initialize
        @generated = []
        @ungenerated = []
        @targets = {}
    end
    def <<(description)
        target = createTarget(description)
        addMethodsToTarget(target)
        @targets[description] = target
        target
    end
    def createTarget(description)
        case description
        when Class then description.new
        else description
        end.tap{|target|@ungenerated << target}
    end
    def addMethodsToTarget(target)
        class << target
            attr_accessor :processor
            def nameContext(name)
            end
            def generate(description)
                target = @processor.find(description)
                unless target 
                    target = @processor << description
                    @processor.process
                end
                target
            end
        end
        target.processor = self
    end

    @@processLevel = 0
    def indent(ch)
        ch*2*@@processLevel
    end
    def process
        @@processLevel += 1
        while target = @ungenerated.shift
            puts(indent(">") + "Generating #{target}")
            target.generate_
            @generated << target
            puts(indent("<") + "Generating #{target}")
        end
        @@processLevel -= 1
    end
    def find(description)
        @targets[description]
    end
end

class Exe
    def initialize(filename)
        @filename = filename
    end
    def generate_
        nameContext :exe
        objects = generate Objects
        settings = generate LinkSettings
    end
end
class Objects
    def generate_
    end
end
class LinkSettings
    def generate_
    end
end

processor = Processor.new
processor << Exe.new("main.cpp")
processor.process
