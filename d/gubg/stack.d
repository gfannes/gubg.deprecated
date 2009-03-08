module gubg.stack;

class Stack(T)
{
    this()
	{
	    mTop = null;
	}
    T* push(T t)
	{
	    Element* element = new Element;
	    element.t = t;
	    element.prev = mTop;
	    mTop = element;
	    return &element.t;
	}
    bool pop(ref T t)
	{
	    if (mTop is null)
		return false;
	    t = mTop.t;
	    Element* tmp = mTop;
	    mTop = mTop.prev;
	    delete tmp;
	    return true;
	}
    bool pop()
	{
	    if (mTop is null)
		return false;
	    Element* tmp = mTop;
	    mTop = mTop.prev;
	    delete tmp;
	    return true;
	}
    void each(void delegate(T t) yield)
	{
	    Element* p = mTop;
	    while (p !is null)
	    {
		yield(p.t);
		p = p.prev;
	    }
	}
    void each(void delegate(T t, uint depth) yield)
	{
	    Element* p = mTop;
	    uint d = 0;
	    while (p !is null)
	    {
		yield(p.t, d);
		p = p.prev;
		++d;
	    }
	}
    uint depth()
	{
	    uint d = 0;
	    Element* p = mTop;
	    while (p !is null)
	    {
		++d;
		p = p.prev;
	    }
	    return d;
	}
    T* top()
	{
	    if (mTop is null)
		return null;
	    return &mTop.t;
	}
private:
    struct Element
    {
	T t;
	Element* prev;
    }
    Element* mTop;
}

version (UnitTest)
{
    import gubg.puts;

    void main()
    {
	auto stack = new Stack!(int);
	int i;

	stack.push(1);
	stack.push(2);
	stack.push(3);
	stack.push(4);
	stack.push(5);
	while (stack.pop(i))
	{
	    puts("i = {}", i);
	}
	stack.push(1);
	stack.push(2);
	stack.push(3);
	stack.push(4);
	stack.push(5);
	while (stack.pop(i))
	{
	    puts("i = {}", i);
	}
    }
}