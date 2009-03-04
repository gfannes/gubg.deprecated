// The composite pattern

interface Interface(I): I
{
    bool getComponent(inout Interface!(I) component, uint ix);
    bool setComponent(Interface!(I) component, uint ix);
}

template Composite(I)
{
    int opApply(int delegate(inout I) dg)
        {
        }
}

template ArrayComposite(I)
{
}

template Leaf(I)
{
    bool getComponent(inout Interface!(I) component, uint ix){return false;}
    bool setComponent(Interface!(I) component, uint ix){return false;}
    mixin Composite!(I);
}


version (Test)
{
    interface Component
    {
        void draw();
    }

    class L: Interface!(Component)
    {
        mixin Leaf!(Component);
        void draw(){}
    }

    class C: Interface!(Component)
    {
        bool getComponent(inout Interface!(Component) component, uint ix)
        {
            return false;
        }
        bool setComponent(Interface!(Component) component, uint ix)
        {
            return false;
        }
        mixin Composite!(Component);
        void draw(){}
    }

    void main()
    {
        L l = new L();
        C c = new C();
    }
}
