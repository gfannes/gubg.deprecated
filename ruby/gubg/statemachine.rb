class StateMachine
    @@verbose = false
    attr_accessor(:receiver)
    attr_reader(:state)
    def initialize(states = {})
        #{entry: callback, exit: callback}
        @infoPerState = Hash.new{|h, s|h[s] = {:entry => nil, :exit => nil}}
        @transitionsPerSignal = Hash.new{|h, s|h[s] = {}}
        states.each do |s, i|
            if :receiver == s
                @receiver = i
            else
                addState(s, i)
            end
        end
    end
    def addState(s, i = nil)
        puts("Adding state #{s}") if @@verbose
        info = @infoPerState[s]
        if i
            i.each do |k, v|
                case k
                when :entry, :exit
                    info[k] = v
                else
                    ary = [v].flatten
                    @transitionsPerSignal[k][s] = {to: ary[0], callback: ary[1]}
                    addState(ary[0])
                end
            end
            if @state.nil?
                @state = s
                execute_(:entry, @infoPerState[@state])
                puts("State is now #{@state}") if @@verbose
            end
        end
    end
    def method_missing(m, *args, &block)
        raise("No state set") if @state.nil?
        execute_(:exit, @infoPerState[@state])
        trans = @transitionsPerSignal[m][@state]
        execute_(:callback, trans, *args, &block)
        @state = trans[:to]
        execute_(:entry, @infoPerState[@state])
        puts("State is now #{@state}") if @@verbose
        nil
    end
    private
    def execute_(sym, info, *args, &block)
        if info[sym]
            puts("Executing #{info[sym]}") if @@verbose
            raise("No receiver is set") if @receiver.nil?
            @receiver.send(info[sym], *args, &block)
        end
    end
end

if __FILE__ == $0
    class Receiver
        def method_missing(m, *args, &block)
            puts("Received #{m}")
        end
    end
    sm = StateMachine.new(receiver: Receiver.new, A: {entry: :e_a, b: :B}, B: {exit: :b_E, a: [:A,:ba]})
    sm.b
    sm.a
end
