#This module provides support for dependency programming
#You start with creating an instance of Breakdown::Global and provide this with a block of top-level targets you want to breakdown
#Each target lives in a context (this instance variable is added), by default, this is your single Global instance
#Each target can create a new scope (Global defines the :global scope) using defineScope (this method is added)
#Each target you add should provide the breakdown_ method that will be called to breakdown it
#Each target can breakdown subtargets (or look them up) using breakdown(description). This method is added to the target
#The description for a target can be:
# * A Class => a single instance will be created in the current context, and this instance is reused if the target is needed multiple times in the same scope
# * An object, no reuse is happening here
module Breakdown
    class ProcessingError < Exception
        def initialize
            super("Processing error")
        end
    end
    class Processor
        def initialize
            @generated = []
            @ungenerated = []
            @targetsPerContext = Hash.new{|h, k|h[k] = {}}
        end
        def createTarget(*description)
            case description.first
            when Class
                klass = description.first
                klass.new(*description[1, description.length])
            else
                description.first
            end
        end
        def addTarget(key, context, target)
            addMethodsToTarget(target)
            target.processor = self
            target.context = context
            @ungenerated << target
            @targetsPerContext[context][key] = target
        end
        def addMethodsToTarget(target)
            class << target
                attr_accessor :processor, :context, :scope, :info
                def defineScope(name)
                    @scope = name
                end
                def breakdown(*args)
                    context = @processor.context(self)
                    key = @processor.key(context, *args)
                    target = @processor.find(key, context)
                    unless target
                        target = @processor.createTarget(*args)
                        @processor.addTarget(key, context, target)
                        raise(Breakdown::ProcessingError) if @processor.process != :ok
                    end
                    raise("Target has problems") if target.info[:state] != :ok
                    target
                end
                def log(msg)
                    puts(@processor.indent(" ", " ")+self.class.to_s+"::"+msg)
                end
            end
        end

        @@processLevel = 0
        def indent(ch, l = "=")
            l*2*(@@processLevel-1) + ch*2 + " "
        end
        def process
            @@processLevel += 1
            while target = @ungenerated.shift
                puts(indent(">") + "#{target.class}::#{target}")
                target.info = {}
                begin
                    target.info[:startTime] = Time.now
                    target.breakdown_
                    target.info[:stopTime] = Time.now
                    target.info[:state] = :ok
                    @generated << target
                rescue ProcessingError => exc
                    target.info[:stopTime] = Time.now
                    target.info[:state] = :error
                    puts(indent("!") + "ProcessingError generating #{target.class}::#{target}")
                    return :error
                rescue => exc
                    target.info[:stopTime] = Time.now
                    target.info[:state] = :error
                    puts(indent("!") + "ERROR generating #{target.class}::#{target}: #{exc.message}\n#{exc.backtrace*"\n"}")
                    return :error
                end
                puts(indent("<") + "#{target.class}::#{target} took #{target.info[:stopTime] - target.info[:startTime]} seconds")
            end
            @@processLevel -= 1
            :ok
        end
        def key(context, *args)
            case args.first
            when Class
                if args.length == 1
                    ("class_" + args.first.to_s).to_sym
                else
                    "key_#{args.first}_#{args.first.key_(context, *args[1, args.length])}"
                end
            else
                "object_#{args.first.class}_#{args.first.object_id}"
            end
        end
        def context(host)
            host.scope ? host : host.context
        end
        def find(key, context)
            @targetsPerContext[context][key]
        end
    end
    class Global
        def initialize(&block)
            @processor = Processor.new
            context = nil
            @processor.addTarget(@processor.key(context, Global), context, self)
            @block = block
        end
        def breakdown_
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
        def breakdown_
            defineScope :exe
            objects = breakdown Objects
            settings = breakdown LinkSettings
        end
    end
    class Objects
        def breakdown_
        end
    end
    class LinkSettings
        def breakdown_
            objects = breakdown Objects
            puts("objects: #{objects}")
        end
    end

    global = Breakdown::Global.new do |global|
        global.breakdown(Exe.new("main1.cpp"))
        global.breakdown(Exe.new("main2.cpp"))
    end
    global.process
end
