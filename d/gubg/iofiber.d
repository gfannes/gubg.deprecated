module gubg.iofiber;

import tango.core.Thread;
import tango.core.Variant;
import gubg.puts;

// This is a more general fiber class that can handle IO between the calling thread and the fiber
// Template parameters:
//  * O: the type of the output
//  * I: the type of the input
// Constructor parameters:
//  * void function(inout O o, I i) or void delegate(inout O o, I i)
//  * Input value of type I
// 
// Call "call()" on the IOFiber object, it will return true untill finished
// Call VariantFiber.yield(T t) in the fiber to yield execution back to the caller AND passing "t" back. Type T should be castable to O.
// Call fiber.output to know the value passed back by the fiber
class IOFiber(O, I): VariantFiber
{
    alias void function(inout O, I) FT;
    alias void delegate(inout O, I) DT;

    this (FT f, I i)
	{
	    mF = f;
	    mInput = i;
            super(&run);
	}
    this (DT d, I i)
	{
	    mD = d;
	    mInput = i;
            super(&run);
	}
    bool reset(I i)
    {
	if (state != State.TERM)
	{
	    version (Test) puts("Fiber is not yet terminated");
	    return false;
	}
	output = O.init;
	mInput = i;
	super.reset();
	return true;
    }
    bool call()
    {
        bool res = false;

	Object obj;

	switch (state)
	{
	case State.HOLD:
	    super.call();
	    res = true;
	    puts("before get");
	    obj = variant.get!(Object);
	    puts("obj = {}", obj);
            output = cast(O)obj;
	    puts("output = {}", output);
	    break;
	case State.TERM:
	    version (Test) puts("Fiber is terminated");
	    break;
	case State.EXEC:
	    version (Test) puts("Fiber is executing");
	    break;
	default:
	    puts("Fiber is in an unknown state");
	    break;
	}

        return res;
    }
    O output;
private:
    void run()
    {
	if (mF is null)
	    mD(output, mInput);
	else
	    mF(output, mInput);
    }
    FT mF;
    DT mD;
    I mInput;
}

class VariantFiber: Fiber
{
    this (void delegate() dg){super(dg);}
    Variant variant;

    static bool yield(T)(T t)
    {
        VariantFiber fiber;
        if ((fiber = cast(VariantFiber)Fiber.getThis) is null)
        {
            version (Test) puts("Yield outside of VariantFiber detected");
            return false;
        }
        fiber.variant = t;
        Fiber.yield;
        return true;
    }
}


version (Test)
{
    void func(inout int o, int i)
    {
	for (int ii = 0; ii < i; ++ii)
	{
	    if (ii > 0)
		VariantFiber.yield(ii);
	    o = ii;
	}
	puts("Fiber is finished");
    }

    class D
    {
	void m(inout int o, int i)
	    {
		puts("Delegate fiber");
		for (int ii = 0; ii < i; ++ii)
		{
		    if (ii > 0)
			VariantFiber.yield(ii);
		    o = ii;
		}
		puts("Fiber is finished");
	    }
    }

    class A
    {
        void m(inout B o, int i)
            {
                C c = new C;
                VariantFiber.yield(c);
            }
    }
    class B{}
    class C:B{}

    void main()
    {
        {
            auto f = new IOFiber!(int, int)(&func, 3);
            while (f.call)
                puts("{}", f.output);
            f.reset(4);
            while (f.call)
                puts("{}", f.output);
        }

        {
            auto d = new D;
            auto f = new IOFiber!(int, int)(&d.m, 3);
            while (f.call)
                puts("{}", f.output);
            f.reset(4);
            while (f.call)
                puts("{}", f.output);
        }

        puts("\nLast");
        {
            auto a = new A;
            auto f = new IOFiber!(B, int)(&a.m, 3);
            while (f.call)
                puts("{}", f.output);
        }

	int o;
	func(o,2);
    }
}
