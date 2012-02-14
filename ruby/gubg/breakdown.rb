#This module provides support for dependency programming
#You start with creating an instance of Breakdown::Global and provide this with a block of top-level targets you want to generate
#Each target lives in a context (this instance variable is added), by default, this is your single Global instance
#Each target can create a new scope (Global defines the :global scope) using defineScope (this method is added)
#Each target you add should provide the generate_ method that will be called to generate it
#Each target can generate subtargets (or look them up) using generate(description). This method is added to the target
#The description for a target can be:
# * A Class => a single instance will be created in the current context, and this instance is reused if the target is needed multiple times in the same scope
# * An object, no reuse is happening here
module Breakdown
    class Processor
        def initialize
            @generated = []
            @ungenerated = []
            @targetsPerContext = Hash.new{|h, k|h[k] = {}}
        end
        def add(description, context)
            target = createTarget(description, context)
            addMethodsToTarget(target)
            target.processor = self
            target.context = context
            target
        end
        def createTarget(description, context)
            case description
            when Class then description.new
            else description
            end.tap{|target|@ungenerated << target; @targetsPerContext[context][description] = target}
        end
        def addMethodsToTarget(target)
            class << target
                attr_accessor :processor, :context, :scope, :info
                def defineScope(name)
                    @scope = name
                end
                def generate(*args)
                    description, context = @processor.resolve(self, *args)
                    target = @processor.find(description, context)
                    unless target 
                        target = @processor.add(description, context)
                        @processor.process
                    end
                    target
                end
            end
        end

        @@processLevel = 0
        def indent(ch)
            "="*2*(@@processLevel-1) + ch*2 + " "
        end
        def process
            @@processLevel += 1
            while target = @ungenerated.shift
                puts(indent(">") + "Generating #{target}")
                target.info = {}
                begin
                    target.info[:startTime] = Time.now
                    target.generate_
                    target.info[:stopTime] = Time.now
                    target.info[:state] = :ok
                    @generated << target
                rescue => exc
                    target.info[:stopTime] = Time.now
                    target.info[:state] = :error
                    puts(indent("!") + "ERROR generating #{target}: #{exc.message}\n#{exc.backtrace*"\n"}")
                    return :error
                end
                puts(indent("<") + "Generating #{target}")
            end
            @@processLevel -= 1
            :ok
        end
        def resolve(host, *args)
            return args.first, (host.scope ? host : host.context)
        end
        def find(description, context)
            @targetsPerContext[context][description]
        end
    end
    class Global
        def initialize(&block)
            @processor = Processor.new
            @processor.add(self, nil)
            @block = block
        end
        def generate_
            defineScope(:global)
            @block.yield(self)
        end
        def process
            @processor.process
        end
    end
end

if __FILE__ == $0
    class Exe
        def initialize(filename)
            @filename = filename
        end
        def generate_
            defineScope :exe
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
            objects = generate Objects
            puts("objects: #{objects}")
        end
    end

    global = Breakdown::Global.new do |global|
        global.generate(Exe.new("main1.cpp"))
        global.generate(Exe.new("main2.cpp"))
    end
    global.process
end
