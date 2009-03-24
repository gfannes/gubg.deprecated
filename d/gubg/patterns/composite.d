module gubg.patterns.composite;
// The composite pattern

import tango.core.Exception;

// Interfaces
enum ReplaceMode
{
    Create,
        Remove,
        Set,
}
interface IComponent(I): I
{
    uint nrComponents();
    void setNrComponents(uint nr);
    IComponent!(I) replaceComponent(IComponent!(I) newComponent, uint ix, ReplaceMode mode);
    IComposite!(I) parent();
    IComposite!(I) parent(IComposite!(I) p);
}
interface IComposite(I): IComponent!(I)
{
}
interface ILeaf(I): IComponent!(I)
{
}

// Templates
template TComposite(I)
{
    // NOTE::Do not forget to set the parent in replaceComponent()
    IComposite!(I) parent(){return mParent;}
    IComposite!(I) parent(IComposite!(I) p)
        {
            mParent = p;
            return mParent;
        }
private:
    IComposite!(I) mParent;
}
template TIndexComposite(I)
{
    IComponent!(I) replaceComponent(IComponent!(I) newComponent, uint ix, ReplaceMode mode)
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
                    newComponent.parent = this;
                this[ix] = newComponent;
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
template TLeaf(I)
{
    uint nrComponents(){return 0;}
    void setNrComponents(uint nr){throw new ArrayBoundsException(__FILE__, __LINE__);}
    IComponent!(I) replaceComponent(IComponent!(I) newComponent, uint ix, ReplaceMode mode)
        {
            throw new ArrayBoundsException(__FILE__, __LINE__);
            return null;
        }
    mixin TComposite!(I);
}


version (UnitTest)
{
    interface ComponentMethods
    {
        void draw();
    }
    alias IComponent!(ComponentMethods) Component;

    class L: ILeaf!(ComponentMethods)
    {
        mixin TLeaf!(ComponentMethods);

        void draw(){}
    }

    class C: IComposite!(ComponentMethods)
    {
        uint nrComponents(){return 0;}
        void setNrComponents(uint nr){}
        Component replaceComponent(Component newComponent, uint ix, ReplaceMode mode)
        {
            throw new ArrayBoundsException(__FILE__, __LINE__);
            return null;
        }
        mixin TComposite!(ComponentMethods);

        void draw(){}
    }

    class IC: IComposite!(ComponentMethods)
    {
        uint nrComponents(){return mArray.length;}
        void setNrComponents(uint nr){mArray.length = nr;}
        Component opIndex(uint ix){return mArray[ix];}
        Component opIndexAssign(Component rhs, uint ix){return (mArray[ix] = rhs);}
        mixin TIndexComposite!(ComponentMethods);

        void draw(){}
    private:
        Component mArray[];
    }

    void main()
    {
        auto l = new L();
        auto c = new C();
        auto ic = new IC();
        ic.replaceComponent(l, 1,ReplaceMode.Create);
        ic.replaceComponent(c, 0,ReplaceMode.Set);
    }
}
