module ulbu.builder;

import tango.io.File;
import tango.text.Text: Text;
import tango.text.convert.Layout: Layout;

import gubg.puts;

import ulbu.element;
import ulbu.primitive;

class Builder
{
    this ()
	{
	    root = new Root;
	    layout = new Layout!(char);
	}

    void build(char[] dirName, char[] fileName)
        {
	    // Create the element for compilation
            auto el = Element.createFrom(dirName, fileName);
	    root.add(el);
	    // Search for the main
	    root.main = null;
	    bool searchMain(Element element)
	    {
		if (element.name == "main")
		{
		    if (root.main !is null)
			throw new Exception("Multiple mains found.");
		    root.main = element;
		}
		return true;
	    }
	    root.depthFirst(&searchMain);
	    // Add the system elements
	    addSystemElements();
	    // Set the locations into the elements
	    root.setLocation();
	    // Resolve the unknown bodies
	    Body[char[]] fn2Body;
	    root.collectBodies(fn2Body);
	    root.resolveBodies(fn2Body);
        }

    void compile(char[] fileName)
        {
	    data = new Text!(char);
	    bss = new Text!(char);
	    text = new Text!(char);

	    data.append("startMessage:
	.ascii \"Starting...\\n\"
endMessage:
	.ascii \"End.\\n\"\n");

	    bss.append(".global _start");

	    text.append("_start:
	movl $12, %edx		# Write start message
	movl $startMessage, %ecx
	movl $1, %ebx
	movl $4, %eax
	int $0x80\n\n");
	    addFunctionCall(root.main);
	    text.append("\n	movl $5, %edx		# Write end message
	movl $endMessage, %ecx
	movl $1, %ebx
	movl $4, %eax
	int $0x80
	
	movl $0, %ebx		# Exit
	movl $1, %eax
	int $0x80
");
            bool addFunctions(Element element)
            {
                if (element.isFunction)
                {
                    text.append(layout("\n{}:\n", element.functionName));
		    text.append(layout("	movl ${}, %edx
	movl ${}, %ecx
	movl $1, %ebx
	movl $4, %eax
	int $0x80\n", element.functionName.length + 1, "_ud." ~ element.functionName));
		    text.append("\tret\n");
		    data.append(layout("_ud.{}:\n\t.ascii \"{}\\n\"\n", element.functionName, element.functionName));
                }
                return true;
            }
            root.depthFirst(&addFunctions);

	    // Write out the assembler program
	    scope fo = new File(fileName);
	    fo.write([]);
	    auto sections = ["data", "bss ", "text"];
	    foreach (ix, buf; [data, bss, text])
	    {
		fo.append(layout("\n.section .{}\n", sections[ix]));
		fo.append(buf.selection);
		fo.append("\n");
	    }
        }

    void print()
	{
	    root.print();
	}

private:
    void addSystemElements()
	{
	    root.add(new Element(new Attributes([]), new Name("int"), new Integer));
	    root.add(new Element(new Attributes([]), new Name("byte"), new Byte));
	}
    void addFunctionCall(Element el)
	{
	    // Add necessary space to stack for output and input
	    text.append(layout("\tsubl ${}, %esp\t# Calling {}\n", el.size(), el.fullName));
	    // Call
	    text.append(layout("\tcall {}\n", el.functionName));
	    // Remove input and output
	    text.append(layout("\taddl ${}, %esp\n", el.size()));
	}

    Root root;
    Layout!(char) layout;
    Text!(char) data ;
    Text!(char) bss;
    Text!(char) text;
}

version (Test)
{
//     import tango.sys.Process: Process;

    void main()
    {
        auto builder = new Builder;

        builder.build("test", "test.ulbu");
	builder.print();
        builder.compile("test.s");
        
//         Process.execute("rm test.o a.out");
//         Process.execute("as -o test.o test.s");
//         Process.execute("ld test.o");
//         Process.execute("./a.out");
    }
}
