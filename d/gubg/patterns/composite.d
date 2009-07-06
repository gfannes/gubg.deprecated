module gubg.patterns.composite;
// The composite pattern

public import gubg.uid;

public import tango.core.Exception;

// Interfaces
enum ReplaceMode
{
    Create,
        Remove,
        Set,
	Get,
}
interface IComponent(I): I
{
    uint nrComponents();
    void setNrComponents(uint nr);
    IComponent!(I) replaceComponent(ReplaceMode mode, uint ix, IComponent!(I) newComponent);
    int opApply(int delegate(IComponent!(I) el) dg);
    IComposite!(I) parent();
    IComposite!(I) parent(IComposite!(I) p);
    bool isLeaf();
    uint uid();
    void compact();
}
interface IComposite(I): IComponent!(I)
{
}
interface ILeaf(I): IComponent!(I)
{
}

bool indexOfParent(T)(inout uint ix, T node)
{
    auto p = node.parent;
    if (p is null)
	return false;
    for (ix = 0; ix < p.nrComponents; ++ix)
	if (p.replaceComponent(ReplaceMode.Get, ix, null).uid == node.uid)
	    break;
    return (ix < p.nrComponents);
}

bool areSiblings(T)(T n1, T n2)
{
    if (n1 is null || n2 is null)
	return false;
    if (n1.uid == n2.uid)
	return false;
    if (n1.parent is null)
	return n2.parent is null;
    if (n2.parent is null)
	return false;
    return n1.parent.uid == n2.parent.uid;
}

bool distanceToRoot(T)(inout uint distance, T from, T to = null)
{
    if (from is null)
	return false;

    bool found = false;
    if (to is null)
    {
	distance = 0;
	auto parent = from.parent;
	while (parent !is null)
	{
	    ++distance;
	    parent = parent.parent;
	}
	found = true;
    } else
    {
	uint targetUID = to.uid;
	uint tmpDistance = 0;
	auto current = from;
	while (current !is null && current.uid != targetUID)
	{
	    ++tmpDistance;
	    current = current.parent;
	}
	if (current !is null)
	{
	    distance = tmpDistance;
	    found = true;
	}
    }
    return found;
}

// Templates
// Provide parent()
template TParent(I)
{
    // NOTE::Do not forget to set the parent in replaceComponent()
    IComposite!(I) parent(){return _parent;}
    IComposite!(I) parent(IComposite!(I) p)
        {
            _parent = p;
            return _parent;
        }
private:
    IComposite!(I) _parent;
}
// Provide uid()
template TUID()
{
    uint uid()
        {
            if (0 == _uid)
                _uid = generateUID("composite");
            return _uid;
        }
private:
    uint  _uid;
}
// Provide compact()
template TCompact(I)
{
    void compact()
        {
	    uint offset = 0;
	    auto nrComp = nrComponents;
	    for (uint i = 0; i < nrComp; ++i)
	    {
		IComponent!(I) node = replaceComponent(ReplaceMode.Get, i, null);
		puts("In compact: i = {}, node = {}, offset = {}", i, cast(void*)node, offset);
		if (node is null)
		    ++offset;
		else if (offset > 0)
		{
		    node = replaceComponent(ReplaceMode.Set, i, null);
		    replaceComponent(ReplaceMode.Set, i - offset, node);
		}
	    }
	    if (offset > 0)
		setNrComponents(nrComp - offset);
        }
private:
    uint  _uid;
}
template TOpApply(I)
{
    int opApply(int delegate(IComponent!(I) el) dg)
	{
	    int res = 0;
	    for (uint i = 0; res == 0 && i < nrComponents; ++i)
		res = dg(replaceComponent(ReplaceMode.Get, i, null));
	    return res;
	}
}
// Composite
template TComposite(I)
{
    mixin TParent!(I);
    mixin TUID;
    mixin TCompact!(I);
    mixin TOpApply!(I);
    bool isLeaf(){return false;}
}
// Provide replaceComponent() and parent() for classes that specify:
//  * opIndex and opIndexAssign
//  * nrComponents and setNrComponents
template TIndexComposite(I)
{
    IComponent!(I) replaceComponent(ReplaceMode mode, uint ix, IComponent!(I) newComponent)
        {
            IComponent!(I) origComponent = null;
            
            uint nrComp = nrComponents;

            switch (mode)
            {
            case ReplaceMode.Create:
                if (ix >= nrComp)
                {
                    nrComp = ix + 1;
                    setNrComponents(nrComp);
                }
                // We fall-through to ReplaceMode.Set
            case ReplaceMode.Set:
                origComponent = this[ix];
                if (origComponent !is null)
                    origComponent.parent = null;
                if (newComponent !is null)
                    newComponent.parent(this);
                this[ix] = newComponent;
                break;

            case ReplaceMode.Get:
                origComponent = this[ix];
                break;

            case ReplaceMode.Remove:
                origComponent = this[ix];
                if (origComponent !is null)
                    origComponent.parent = null;
                for (uint i = ix + 1; ix < nrComp; ++ix)
                    this[i-1] = this[i];
                --nrComp;
                this[nrComp] = null;
                setNrComponents(nrComp);
                break;
            }

            return origComponent;
        }

    mixin TComposite!(I);
}
// Provide parent() and disabled replaceComponent() functionality
template TLeaf(I)
{
    uint nrComponents(){return 0;}
    void setNrComponents(uint nr){throw new ArrayBoundsException(__FILE__, __LINE__);}
    IComponent!(I) replaceComponent(ReplaceMode mode, uint ix, IComponent!(I) newComponent)
        {
            throw new ArrayBoundsException(__FILE__, __LINE__);
            return null;
        }
    bool isLeaf(){return true;}
    int opApply(int delegate(IComponent!(I) el) dg)
	{
	    return 0;
	}
    mixin TParent!(I);
    mixin TUID;
    void compact(){}
}


version (UnitTest)
{
    // We want all our components to provide draw()
    interface ComponentMethods
    {
        void draw();
    }
    // Component is our ComponentMethods interface, extended with the typical component methods
    alias IComponent!(ComponentMethods) Component;

    // A leaf component
    class L: ILeaf!(ComponentMethods)
    {
        mixin TLeaf!(ComponentMethods);

	// ComponentMethods
        void draw(){}
    }

    // A general composite component
    class C: IComposite!(ComponentMethods)
    {
        uint nrComponents(){return 0;}
        void setNrComponents(uint nr){}
        Component replaceComponent(ReplaceMode mode, uint ix, Component newComponent)
        {
            throw new ArrayBoundsException(__FILE__, __LINE__);
            return null;
        }
        mixin TComposite!(ComponentMethods);

	// ComponentMethods
        void draw(){}
    }

    // A composite component which provides opIndex and opIndexAssign
    class IC: IComposite!(ComponentMethods)
    {
        uint nrComponents(){return _array.length;}
        void setNrComponents(uint nr){_array.length = nr;}
        Component opIndex(uint ix){return _array[ix];}
        Component opIndexAssign(Component rhs, uint ix){return (_array[ix] = rhs);}
        mixin TIndexComposite!(ComponentMethods);

	// ComponentMethods
        void draw(){}
    private:
        Component[] _array;
    }

    void main()
    {
        auto l = new L();
        auto c = new C();
        auto ic = new IC();
	ic.replaceComponent(ReplaceMode.Create, 1, l);
	ic.replaceComponent(ReplaceMode.Set, 0, c);
    }
}
