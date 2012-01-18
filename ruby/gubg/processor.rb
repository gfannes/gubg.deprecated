require("monitor")

#Executes a job in parallel on a predefined number of threads
#You job should have the execute method
class Processor
    def initialize(nrThreads)
        @jobs = []
        @nrExecuting = 0
        @jobs.extend(MonitorMixin)
        @jobArrived, @jobExecuted = @jobs.new_cond, @jobs.new_cond
        @workers = []
        nrThreads.times do
            @workers << Thread.new do
                loop do
                    job = nil
                    @jobs.synchronize do
                        @jobArrived.wait_while{@jobs.empty?}
                        job = @jobs.shift
                        @nrExecuting += 1
                    end
                    job.execute
                    @jobs.synchronize do
                        @nrExecuting -= 1
                        @jobExecuted.signal
                    end
                end
            end
        end
    end
    def <<(job)
        @jobs.synchronize do
            @jobs << job
            @jobArrived.signal
        end
    end
    def waitUntilIdle
        @jobs.synchronize do
            return if @jobs.empty?
            @jobExecuted.wait_while{!@jobs.empty? or @nrExecuting > 0}
            return if @jobs.empty?
        end
    end
end

if __FILE__ == $0
    class Job
        def initialize(id)
            @id = id
        end
        def execute
            puts("Starting job #{@id}")
            sleep rand
            puts("Stopping job #{@id}")
        end
    end
    processor = Processor.new(4)
    100.times{|id|processor << Job.new(id)}
    processor.waitUntilIdle
end
