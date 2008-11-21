module ditf.builder;

import ditf.element;

import gubg.parser.language;
import gubg.parser.tokenSequence;
import gubg.parser.token;

version (Test)
{
    import gubg.file;
    import gubg.puts;
    
    void main()
    {
	char[] sourceCode;
	loadFile(sourceCode, "data/test.ditf");
	auto ts = new TokenSequence!(DITFLanguage)(sourceCode);
	
	foreach (token; ts)
	    puts("token({}) = \"{}\"", token.known, token.str);
	puts("Nr tokens = {}", ts.length);
    }
}
