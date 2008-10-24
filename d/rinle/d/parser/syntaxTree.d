module rinle.d.parser.syntaxTree;

import rinle.d.parser.tokenSequence;
import rinle.d.parser.construct;
import gubg.puts;

class SyntaxTree
{
    this (char[] sourceCode)
        {
            TokenSequence ts = new TokenSequence(sourceCode);
	    sf = SourceFile.create(ts);
        }

    SourceFile sf;
}

version (Test)
{
    import gubg.file;

    void main()
    {
        char[] sourceCode;
        loadFile(sourceCode, "test.d");
        SyntaxTree st = new SyntaxTree(sourceCode);
    }
}
