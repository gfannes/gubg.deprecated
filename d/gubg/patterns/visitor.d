interface IVisitor
{
    void visit(T)(inout T visited);
    void f(inout int i);
}

interface IVisited
{
    void accept(inout IVisitor visitor);
}

template Visited()
{
    void accept(inout IVisitor visitor)
        {
//            visitor.visit!(typeof(this))(this);
        }
}

version (Test)
{
    import gubg.puts;

    class A: IVisited
    {
        mixin Visited;
    }

    class B: IVisitor
    {
//         void visit(inout A a)
//         {
//             puts("Visiting A");
//         }
        void visit(T: int)(inout int i)
        {
        }
        void f(inout int i)
        {
        }
    }

    void main()
    {
        IVisitor b = new B;
        int i;
        b.visit!(int)(i);
    }
}
