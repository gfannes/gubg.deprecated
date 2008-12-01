module ulbu.builder;

import tango.text.convert.Layout: Layout;

import gubg.puts;

import ulbu.element;
import ulbu.primitive;
import ulbu.assembler;

class Builder
{
    this ()
	{
	    root = new Root;
	    layout = new Layout!(char);
	}

    void build(char[] dirName, char[] fileName)
        {
            puts("Building ...");
	    // Create the element for compilation
            auto el = BodyElement.createFrom(dirName, fileName);
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
            puts("Searching for main");
	    root.depthFirst(&searchMain);
	    // Add the system elements
            puts("Adding system elements");
	    addSystemElements();
	    // Set the locations into the elements
            puts("Setting the location");
	    root.setLocation();
	    // Resolve the unknown bodies
            puts("Resolving bodies");
	    BodyDefinition[char[]] fn2Body;
	    puts("\tCollecting");
	    root.collectBodies(fn2Body);
	    puts("\tResolving");
	    root.resolveBodies(fn2Body);
            puts("... building finished");
        }

    void compile(char[] fileName)
        {
            puts("Compilation ...");
	    asmFile = new AsmFile;
	    
	    asmFile.addData("startMessage", "ascii", "Starting...\\n");
	    asmFile.addData("endMessage", "ascii", "End.\\n");
// 	    asmFile.addData("callDepth", "int", 0);
// 	    asmFile.addData("indent", "ascii", "\t");
// 	    asmFile.addData("enterMessage", "ascii", "Entering ");
// 	    asmFile.addData("leaveMessage", "ascii", "Leaving  ");

	    asmFile.addBSS(".global _start");

	    asmFile.newFunction("_start"); // _start
	    asmFile.add("movl $12, %edx", "Write start message");
	    asmFile.add("movl $startMessage, %ecx");
	    asmFile.add("movl $1, %ebx");
	    asmFile.add("movl $4, %eax");
	    asmFile.add("int $0x80");
	    addFunctionCall(root.main); // Call main
	    asmFile.add("movl $5, %edx", "Write end message");
	    asmFile.add("movl $endMessage, %ecx");
	    asmFile.add("movl $1, %ebx");
	    asmFile.add("movl $4, %eax");
	    asmFile.add("int $0x80");
	
	    asmFile.add("movl $0, %ebx", "Exit");
	    asmFile.add("movl $1, %eax");
	    asmFile.add("int $0x80");

            bool addFunctions(Element element)
            {
                if (element.isFunction)
                {
		    auto dataName = layout("_ud.{}", element.functionName);
		    asmFile.addData(dataName, "ascii", element.functionName ~ "\n");

		    asmFile.newFunction(element.functionName);
                    // Function prologue
		    asmFile.add(layout("movl ${}, %edx", element.functionName.length + 1), "Function prologue");
		    asmFile.add(layout("movl ${}, %ecx", dataName));
		    asmFile.add("movl $1, %ebx");
		    asmFile.add("movl $4, %eax");
		    asmFile.add("int $0x80");
                    // Add the function instructions
                    element.addFunctionInstructions(asmFile);
                    // Function epilogue
		    asmFile.add(layout("movl ${}, %edx", element.functionName.length + 1), "Function epilogue");
		    asmFile.add(layout("movl ${}, %ecx", dataName));
		    asmFile.add("movl $1, %ebx");
		    asmFile.add("movl $4, %eax");
		    asmFile.add("int $0x80");
                    // Return
		    asmFile.add("ret");
                }
                return true;
            }
            root.depthFirst(&addFunctions);

	    // Write out the asm file
	    asmFile.write(fileName);
            puts("... compilation finished.");
        }

    void print()
	{
            puts("Printing ...");
	    root.print();
            puts("... printing finished.");
	}

private:
    void addSystemElements()
	{
	    root.add(new BodyElement(new Attributes([]), new Name("int"), new Integer));
	    root.add(new BodyElement(new Attributes([]), new Name("byte"), new Byte));
	}
    void addFunctionCall(Element el)
	{
	    // Add necessary space to stack for output and input
	    asmFile.add(layout("subl ${}, %esp", el.size), layout("Calling {}", el.fullName));
	    // Call
	    asmFile.add(layout("call {}", el.functionName));
	    // Remove input and output
	    asmFile.add(layout("addl ${}, %esp", el.size));
	}

    Root root;
    AsmFile asmFile;
    Layout!(char) layout;
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
