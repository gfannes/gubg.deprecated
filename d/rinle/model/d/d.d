module rinle.model.d.d;

public import rinle.model.interfaces;

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

class DIdentifier: ILeafNode
{
    this (char[] identifier)
    {
	_identifier = identifier.dup;
    }

    void addTo(inout FormatTree ft)
    {
    }
    void expand()
    {
    }
    mixin TLeaf!(NodeMethods);

private:
    char[] _identifier;
}

class DScope: ICompositeNode
{
    uint nrComponents(){return _declarations.length;}
    void setNrComponents(uint nr){_declarations.length = nr;}
    INode opIndex(uint ix){return _declarations[ix];}
    INode opIndexAssign(INode rhs, uint ix)
    {
        DDeclaration declaration = cast(DDeclaration)rhs;
        if (declaration is null)
            throw new Exception("Assignment of non-DDeclaration to DScope.");
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
    DDeclaration[] _declarations;
}

class DModuleDeclaration: DDeclaration
{
    this (char[] name)
    {
	_name = name.dup;
    }
private:
    char[] _name;
}

class DImportDeclaration: DDeclaration
{
    this (char[] name)
    {
	_name = name.dup;
    }
private:
    char[] _name;
}

class DMixinDeclaration: DDeclaration
{
    this (char[] name)
    {
	_name = name.dup;
    }
private:
    char[] _name;
}

class DClassDeclaration: DDeclaration
{
    void setName(DIdentifier name)
    {
	_name = name;
    }
private:
    DIdentifier _name;
    DScope _body;
}

class DBaseClasses: ICompositeNode
{
    uint nrComponents(){return _baseClasses.length;}
    void setNrComponents(uint nr){_baseClasses.length = nr;}
    INode opIndex(uint ix){return _baseClasses[ix];}
    INode opIndexAssign(INode rhs, uint ix)
    {
	DIdentifier baseClass = cast(DIdentifier)rhs;
        if (baseClass is null)
            throw new Exception("Assignment of non-DIdentifier to DBaseClasses.");
        return (_baseClasses[ix] = baseClass);
    }
    mixin TIndexComposite!(NodeMethods);

    void addTo(inout FormatTree ft)
    {
    }
    void expand()
    {
    }
private:
    DIdentifier[] _baseClasses;
}

version (UnitTest)
{
    void main()
    {
    }
}
