require("gubg/statemachine")
require("gubg/graph")

module GUBG
    module Target
        def stateMachine
            StateMachine.new({
                idle:     {                process: :p_expand},
                p_expand: {entry: :expand, process: :p_check,                   error: :error, newTarget: :p_wait,},
                p_wait:   {                                                     error: :error, ready: :p_expand},
                p_check:  {entry: :check, allReady: :ready, notAllReady: :wait, error: :error},
                p_ready:  {                process: [:ready, :generate],        error: :error},
                ready:    {entry: :targetReady},
                error:    {entry: :stop},
            })
        end
        def expand
            puts("Expanding")
            expand_
        end
    end
    module OldTarget
        attr_reader(:generationState)
        def defineDependencies(klassPerName = {})
            @klassPerName = klassPerName
            @namePerKlass = klassPerName.invert
            raise("There are duplicates in the dependency list for #{self.class}: #{klassPerName}") if @klassPerName.length != @namePerKlass.length
        end
        def dependsOn?(otherTarget)
            raise("Did you forget to define the dependencies for class #{self.class}? Define them using defineDependencies, even if they are empty.") if @namePerKlass.nil?
            @namePerKlass.has_key?(otherTarget.class)
        end
        def setDependencies(targets)
            @targetPerName = {}
            @namePerKlass.each do |klass, name|
                target = targets.find{|target|klass === target}
                raise("Could not find the target of class #{klass}") if target.nil?
                @targetPerName[name] = target
            end
            raise("Some required targets are missing for #{self.class}") if @targetPerName.length != @klassPerName.length
        end
        def getTarget(name)
            @targetPerName[name]
        end
        def getTargets(*names)
            case names.length
            when 0 then return *@targetPerName.values
            when 1 then return @targetPerName[names[0]]
            else
                return *names.map{|n|@targetPerName[n]}
            end
        end

        STATES = [:waitingForDependencies, :firstTime, :generating, :halted, :generated, :error]
        def setGenerationState(generationState)
            raise("I cannot accept generationState #{generationState}, it is not part of #{STATES}") if !STATES.include?(generationState)
            @generationState = generationState
        end
        def generationState?(wantedState)
            @generationState == wantedState
        end
        #You have to provide generate_ yourself. We cannot provide a dummy implementation because including Target will override
        #any default implementation that is provided via inheritance
        def generate
            setGenerationState(:firstTime) if generationState?(:waitingForDependencies)
            puts(">>>> #{self.class}")
            setGenerationState(generate_)
            puts("<<<< #{self.class}")
        end
        #Provide a progressible? when you have cyclic dependencies that can gradually be solved
        def print
            puts("Target #{self.class}")
        end
    end

    class TargetGraph < Graph
        def initialize(*targets)
            super()
            targets.each{|target|add(target)}
            #Add the edges that define the target graph. An edge points to a dependency.
            targets.each do |target|
                targets.each do |depTarget|
                    addEdge(target, depTarget) if target.dependsOn?(depTarget)
                end
                target.setDependencies(outVertices(target))
                target.setGenerationState(:waitingForDependencies)
            end if false
        end
        def add(target)
        end
        def process
            #Collect all unfinished targets
            ungeneratedTargets = vertices.reject{|target|target.generationState?(:generated)}
            return :finished if ungeneratedTargets.empty?

            #Collect all targets that can be generated directly, i.e., all dependent targets are generated
            executableTargets = ungeneratedTargets.select{|target|outVertices(target).all?{|depTarget|depTarget.generationState?(:generated)}}
            if !executableTargets.empty?
                puts("\tThe following targets are directly generatable #{executableTargets.map{|target|target.class}}")
                executableTargets.each do |target|
                    target.generate
                end
                failedTargets = executableTargets.select{|target|!target.generationState?(:generated)}
                if !failedTargets.empty?
                    if failedTargets.all?{|target|target.generationState?(:error)}
                        return :error
                    else
                        raise("I expected all executable targets to be generated by now, but these are not: #{failedTargets.map{|target|target.class}}")
                    end
                end
                return :direct
            end

            #Collect all progressible targets
            progressibleTargets = ungeneratedTargets.select{|target|target.progressible?}
            if !progressibleTargets.empty?
                if progressibleTargets.all?{|target|target.generationState?(:halted)}
                    #All targets are halted, we give them a chance to become generated, else we stop
                    progressibleTargets.each do |target|
                        target.generate
                    end
                    return :halted if progressibleTargets.all?{|target|target.generationState?(:halted)}
                elsif progressibleTargets.any?{|target|target.generationState?(:error)}
                    progressibleTargets.each do |target|
                        target.print if target.generationState?(:error)
                    end
                    return :error
                else
                    puts("\tThe following targets are implicitly generatable #{progressibleTargets.map{|target|target.class}}")
                    progressibleTargets.each do |target|
                        target.generate
                    end
                end
                return :implicit
            end

            puts("No more progress can be made, everything is blocked")
            return :blocked
        end
        def getTargets(wantedState)
            vertices.select{|target|target.generationState?(wantedState)}
        end
    end
end

if __FILE__ == $0
    include GUBG
    class A
        def expand_
        end
    end

    tg = TargetGraph.new
    tg.add(A.new)
end
