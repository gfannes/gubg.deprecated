module rinle.model.interfaces;

import gubg.patterns.composite;

interface NodeMethods
{
    
}
alias IComponent!(NodeMethods) INode;
alias IComposite!(NodeMethods) ICompositeNode;
alias ILeaf!(NodeMethods) ILeafNode;
