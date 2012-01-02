require("gubg/graph")

module Task
    attr_reader(:state)
    def defineDependencies(klassPerName = {})
        @klassPerName = klassPerName
        @namePerKlass = klassPerName.invert
        raise("There are duplicates in the dependency list for #{self.class}: #{klassPerName}") if @klassPerName.length != @namePerKlass.length
    end
    def dependsOn?(otherTask)
            raise("Did you forget to define the dependencies for class #{self.class}? Define them using defineDependencies, even if they are empty.") if @namePerKlass.nil?
            @namePerKlass.has_key?(otherTask.class)
    end
    def setDependencies(tasks)
        @taskPerName = {}
        @namePerKlass.each do |klass, name|
            task = tasks.find{|task|klass === task}
            raise("Could not find the task of class #{klass}") if task.nil?
            @taskPerName[name] = task
        end
        raise("Some required tasks are missing for #{self.class}") if @taskPerName.length != @klassPerName.length
    end
    def getTask(name)
        @taskPerName[name]
    end
    STATES = [:waitingForDependencies, :executing, :halted, :executed]
    def setState(state)
        raise("I cannot accept state #{state}, it is not part of #{STATES}") if !STATES.include?(state)
        @state = state
    end
    def state?(wantedState)
        @state == wantedState
    end
    def execute
        setState(:executing) if state?(:waitingForDependencies)
        setState(execute_)
    end
    def execute_
        raise("You have to implement the execute_ method for #{self.class}")
    end
    #Override this method when you have cyclic dependencies that can gradually be solved
    def progressible?
        false
    end
    def print
        puts("Task #{self.class}")
    end
end

class TaskGraph < Graph
    def initialize(tasks)
        super()
        #Add the edges that define the task graph. An edge points to a dependency.
        tasks.each do |task|
            tasks.each do |depTask|
                addEdge(task, depTask) if task.dependsOn?(depTask)
            end
            task.setDependencies(outVertices(task))
            task.setState(:waitingForDependencies)
        end
    end
    def process
        #Collect all unfinished tasks
        unexecutedTasks = vertices.reject{|task|task.state?(:executed)}
        return :finished if unexecutedTasks.empty?

        #Collect all tasks that can be executed directly, i.e., all dependent tasks are executed
        executableTasks = unexecutedTasks.select{|task|outVertices(task).all?{|depTask|depTask.state?(:executed)}}
        if !executableTasks.empty?
            puts("\tThe following tasks are executable #{executableTasks.map{|task|task.class}}")
            executableTasks.each do |task|
                task.execute
                task.print
            end
            failedTasks = executableTasks.select{|task|task.state != :executed}
            raise("I expected all executable tasks to be executed by now, but these are not: #{failedTasks.map{|task|task.class}}") if !failedTasks.empty?
            return :executing
        end

        #Collect all progressible targets
        progressibleTasks = unexecutedTasks.select{|task|task.progressible?}
        if !progressibleTasks.empty?
            if progressibleTasks.all?{|task|task.state?(:halted)}
                #All targets are halted, we give them a chance to become generated, else we stop
                progressibleTasks.each do |task|
                    task.execute
                    task.print
                end
                return :halted if progressibleTasks.all?{|task|task.state?(:halted)}
            elsif progressibleTasks.any?{|task|task.state?(:error)}
                progressibleTasks.each do |task|
                    task.print if task.state?(:error)
                end
                return :error
            else
                puts("\tThe following tasks are progressible #{progressibleTasks.map{|task|task.class}}")
                progressibleTasks.each do |task|
                    task.execute
                    task.print
                end
            end
            return :progressing
        end

        puts("No more progress can be made, everything is blocked")
        return :blocked
    end
end

if __FILE__ == $0
    class A
        include Task
        def initialize
            defineDependencies()
        end
        def execute_
            :executed
        end
    end
    class B
        include Task
        def initialize
            defineDependencies(a: A)
        end
        def execute_
            :executed
        end
    end
    class C
        include Task
        def initialize
            defineDependencies(b: B, d: D)
        end
        def execute_
            getTask(:d).state?(:executed) ? :executed : :halted
        end
        def progressible?
            getTask(:b).state?(:executed)
        end
    end
    class D
        include Task
        def initialize
            defineDependencies(b: B, c: C)
        end
        def execute_
            :executed
        end
        def progressible?
            getTask(:b).state?(:executed)
        end
    end

    tasks = [A, B, C, D].map{|klass|klass.new}
    taskGraph = TaskGraph.new(tasks)

    cycleCount = -1
    loop do
        cycleCount += 1
        puts("\nRunning cycle #{cycleCount}")
        res = taskGraph.process
        puts("\tCycle #{cycleCount} resulted in #{res}")
        case res
        when :executing
        when :progressing
        else
            break
        end
    end
end
