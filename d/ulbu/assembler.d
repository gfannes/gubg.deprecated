module ulbu.assembler;

import tango.io.File;
import tango.text.Text: Text;
import tango.text.convert.Layout: Layout;
import tango.text.Util;

class AsmFile
{
    this ()
	{
	    mData = new AsmSection("data");
	    mBSS = new AsmSection("bss");
	    mText = new AsmSection("text");
	    layout = new Layout!(char);
	}

    void write(char[] fileName)
	{
	    scope fo = new File(fileName);
	    fo.write([]);
	    foreach (section; [mData, mBSS, mText])
	    {
		section.write(fo);
	    }
	}

    void addData(char[] name, char[] type, char[] value)
	{
	    auto str = name ~ ":\n\t." ~ type ~ "\t";
	    switch (type)
	    {
	    case "ascii":
		value = substitute(value.dup, "\t", "\\t");
		value = substitute(value, "\n", "\\n");
		mData.add(str ~ "\"" ~ value ~ "\"");
		break;
	    default:
		mData.add(str ~ value);
		break;
	    }
	}
    void addData(char[] name, char[] type, int value)
	{
	    mData.add(layout("{}:\n\t.{}\t{}", name, type, value));
	}
    void addBSS(char[] str){mBSS.add(str);}
    void newFunction(char[] functionName){mText.add(functionName ~ ":");}
    void add(char[] instr, char[] comment = "")
	{
	    if (comment == "")
		mText.add("\t" ~ instr);
	    else
		mText.add("\t" ~ instr ~ "\t# " ~ comment);
	}

private:
    AsmSection mData;
    AsmSection mBSS;
    AsmSection mText;
    Layout!(char) layout;
}

class AsmSection
{
    this (char[] type)
	{
	    mType = type;
	    layout = new Layout!(char);
	    text = new Text!(char);
	}

    void add(char[] str)
	{
	    text.append(str);
	    text.append("\n");
	}

    void write(File fo)
	{
	    fo.append(layout("\n.section .{}\n", mType));
	    fo.append(text.selection);
	}

private:
    char[] mType;
    Layout!(char) layout;
    Text!(char) text;
}

version (Test)
{
    void main()
    {
	auto asmFile = new AsmFile();

	asmFile.addBSS(".global _start");

	asmFile.write("test.s");
    }
}
