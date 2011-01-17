#The exception class we will throw when an abstract method is called
class AbstractMethodError < RuntimeError
    attr(:methodName)
    def initialize(methodName)
        @methodName = methodName
        super("Abstract method \"#{@methodName}\" was called.")
    end
end

class Module
    def abstract(methodName)
        define_method(methodName){|*parameters|raise(AbstractMethodError, methodName)}
    end
end

if $0 == __FILE__
    require("test/unit")
    class AbstractClass
        abstract(:foo)
        abstract(:bar)
    end
    class ConcreteClass < AbstractClass
        def foo
        end
        def bar(a, b, c)
        end
    end
    class TestCase < Test::Unit::TestCase
        def test_negAbstractClass
            assert_raise(AbstractMethodError){AbstractClass.new.foo}
            assert_raise(AbstractMethodError){AbstractClass.new.bar}
            assert_raise(AbstractMethodError){AbstractClass.new.bar(1, 2, 3)}
        end
        def test_posConcreteClass
            ConcreteClass.new.foo
            ConcreteClass.new.bar(1, 2, 3)
        end
        def test_negConcreteClass
            assert_raise(ArgumentError){ConcreteClass.new.bar}
        end
    end
end
