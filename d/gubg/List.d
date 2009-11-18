module gubg.List;

import gubg.Puts;

class List(T)
{
    this (){}
    
    this (T[] values)
	{
	    add(values);
	}

    void add(T value)
	{
	    auto el = new Element(value);
	    el.prev = tail;
	    if (tail is null)
		head = el;
	    else
		tail.next = el;
	    tail = el;
	}

    void add(T[] values)
	{
	    foreach (v; values)
		add(v);
	}

    int opApply(int delegate(inout T) dg)
	{
	    int res = 0;
	    Element el = head;
	    
	    while (el !is null)
	    {
		if (0 != (res = dg(el.value)))
		    break;
		el = el.next;
	    }

	    return res;
	}

    void each(bool delegate(T v, T* next) dg)
	{
	    T v;
	    T* next;
	    Element el = head;
	    

	    if (el !is null)
	    {
		next = &el.value;
		el = el.next;
	    }

	    bool cont = true;
	    while (el !is null)
	    {
		v = *next;
		next = &el.value;
		if (!dg(v, next))
		{
		    cont = false;
		    break;
		}
		el = el.next;
	    }
	    if (cont && next !is null)
		dg(*next, null);
	}

    uint length()
	{
	    uint nr = 0;
	    Element el = head;
	    while (el !is null)
	    {
		++nr;
		el = el.next;
	    }
	    return nr;
	}

    void handover(List!(T) dest)
	{
	    dest.head = head;
	    dest.tail = tail;
	    head = tail = null;
	}

    bool empty(){return head is null;}

    T pop()
	{
	    T res;
	    if (head !is null)
	    {
		res = head.value;
		head = head.next;
		if (head is null)
		    tail = null;
	    }
	    return res;
	}

    T peep()
	{
	    T res;
	    if (head !is null)
		res = head.value;
	    return res;
	}

    SavePoint savePoint()
	{
	    SavePoint sp = new SavePoint(head);
	    sp.next = savePoints;
	    savePoints = sp;
	    return sp;
	}

    void commit()
	{
	    if (head !is null)
		head.prev = null;
	    SavePoint sp = savePoints;
	    while (sp)
		sp.invalidate;
	}

    class Element
    {
	this(T v)
	    {
		value = v;
	    }

	Element prev;
	Element next;
	T value;
    }

    class SavePoint
    {
	this (Element el)
	    {
		valid = true;
		restoreElement = el;
	    }
	~this ()
	    {
		if (savePoints != this)
		    throw new Exception("I expected another savepoint. Maybe you forgot to scope one or did not respect the order");
		SavePoint sp = savePoints;
		savePoints = savePoints.next;
	    }
	bool restore()
	    {
		bool res = valid;
		if (valid)
		{
		    head = restoreElement;
		    if (tail is null && head !is null)
		    {
			tail = head;
			while (tail.next !is null)
			    tail = tail.next;
		    }
		}
		return res;
	    }
	void invalidate(){valid = false;}

	SavePoint next;
	bool valid;
	Element restoreElement;
    }

    Element head;
    Element tail;
    SavePoint savePoints;
}

version (UnitTest)
{
    import gubg.Puts;
    void main()
    {
	auto list = new List!(int);

	void print()
	{
	    foreach (v; list)
		puts("{}", v);
	}

	list.add(1);
	list.add(2);
	list.add(3);
	list.add([4,5,6]);
	    
	print();

	{
	    scope sp = list.savePoint;
	    list.pop;
	    list.pop;
	    list.pop;
	    print();
	    sp.restore;
	    print();
	}
	puts("OK");
    }
}