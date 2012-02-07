module Statemachine
    def setup tt
        @transitions = tt.dup
        @state = @transitions.first.first
    end
    def state
        @state
    end
    def state? s
        @state == s
    end
    def method_missing(m)
        res = send "#{m}_".to_sym, @state
        raise "State #{@state} is a final state and cannot handle any signal" unless @transitions.has_key? @state
        raise "State #{@state} cannot handle signal #{m}" unless @transitions[@state].has_key? m
        raise "State #{res} is not allowed from #{@state} via signal #{m}" unless @transitions[@state][m].include? res
        puts "Changing state #{@state} to #{res} via #{m}"
        @state = res
    end
end

if __FILE__ == $0
    class SM
        include Statemachine
        def initialize tt
            setup tt
        end
        def split_(s)
            :split
        end
        def process_(s)
            case s
            when :split then :generating
            else
                :generated
            end
        end
    end
    tt = {
        fresh: {split: [:split, :error]},
        split: {process: [:generating, :generated, :halted, :error]},
        generating: {process: [:generated, :halted, :error]},
        halted: {unlock: [:generated, :error]},
    }
    sm = SM.new tt
    sm.split
    sm.process until sm.state? :generated
end
