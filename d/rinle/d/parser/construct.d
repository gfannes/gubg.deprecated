import gubg.list;
import rinle.d.parser.token;

interface IConstruct
{
    IConstruct create(inout List!(Token) list);
}

class SourceFile
{
    this (){}

    static SourceFile create(List!(Token) list)
	{
	    SourceFile res;



	    return res;
	}
}
