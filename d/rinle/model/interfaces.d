module rinle.model.interfaces;

import rinle.formatTree;

import gubg.patterns.composite;

interface NodeMethods
{
    void addTo(inout FormatTree ft);
}
alias IComponent!(NodeMethods) INode;
alias IComposite!(NodeMethods) ICompositeNode;
alias ILeaf!(NodeMethods) ILeafNode;
