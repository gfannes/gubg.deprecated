// The composite pattern

// Interfaces
interface IComponent(I): I
{
    bool getComponent(inout IComponent!(I) component, uint ix);
    bool setComponent(IComponent!(I) component, uint ix);
    bool getParent(inout IComposite!(I) parent);
    void setParent(IComposite!(I) parent);
}
interface IComposite(I): I, IComponent!(I)
{
}
interface ILeaf(I): I, IComponent!(I)
{
}

// Templates
template TComposite(I)
{
    // getComponent and setComponent should be implemented
    bool getParent(inout IComposite!(I) parent)
        {
            parent = mParent;
            return (parent !is null);
        }
    void setParent(IComposite!(I) parent)
        {
            mParent = parent;
        }
private:
    IComposite!(I) mParent;
}
template TArrayComposite(I)
{
    // getComponent and setComponent use the index operations
    bool getComponent(inout IComponent!(Component) component, uint ix)
        {
            bool found = true;
            try
            {
                component = this[ix];
            }
            catch (ArrayBoundsError)
            {
                found = false;
                component = null;
            }
            return found;
        }
    bool setComponent(IComponent!(Component) component, uint ix)
        {
            bool ok = true;
            try
            {
                this[ix] = component;
            }
            catch (ArrayBoundsError)
            {
                ok = false;
            }
            return ok;
        }
    mixin TComposite!(I);
}
template TLeaf(I)
{
    // getComponent and setComponent always return false
    bool getComponent(inout IComponent!(I) component, uint ix){return false;}
    bool setComponent(IComponent!(I) component, uint ix){return false;}
    mixin TComposite!(I);
}


version (UnitTest)
{
    interface Component
    {
        void draw();
    }

    class L: ILeaf!(Component)
    {
        mixin TLeaf!(Component);
        void draw(){}
    }

    class C: IComposite!(Component)
    {
        bool getComponent(inout IComponent!(Component) component, uint ix)
        {
            return false;
        }
        bool setComponent(IComponent!(Component) component, uint ix)
        {
            return false;
        }
        mixin TComposite!(Component);

        void draw(){}
    }

    void main()
    {
        L l = new L();
        C c = new C();
    }
}
