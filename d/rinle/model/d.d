module rinle.model.d;

import rinle.model.interfaces;

class DModule: ICompositeNode
{
    this (char[] path, char[] name)
    {
        _path = path.dup;
        _name = name.dup;
    }

    uint nrComponents(){return _declarations.length;}
    void setNrComponents(uint nr){_declarations.length = nr;}
    INode opIndex(uint ix){return _declarations[ix];}
    INode opIndexAssign(INode rhs, uint ix)
    {
        DDeclaration declaration = cast(DDeclaration)rhs;
        if (declaration is null)
            throw new Exception("Assignment of non-DDeclaration to DModule.");
        return (_declarations[ix] = declaration);
    }
    mixin TIndexComposite!(NodeMethods);

    void addTo(inout FormatTree ft)
    {
    }
    void expand()
    {
    }

private:
    char[] _path;
    char[] _name;

    DDeclaration[] _declarations;
}

class DDeclaration: ILeafNode
{
    void addTo(inout FormatTree ft)
    {
    }
    void expand()
    {
    }

    mixin TLeaf!(NodeMethods);
}
