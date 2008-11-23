module ulbu.builder;

import ulbu.element;

import gubg.puts;

class Builder
{
    void build(inout TS ts)
	{
	    foreach (token; ts)
		puts("token({}) = \"{}\"", token.known, token.str);
	    puts("Nr tokens = {}", ts.length);
	}
}

version (Test)
{
    import gubg.file;
    import gubg.puts;
    
    void main()
    {
	char[] sourceCode;
	loadFile(sourceCode, "data/test.ulbu");
	auto ts = new TS(sourceCode);

	auto builder = new Builder;

	builder.build(ts);
    }
}
