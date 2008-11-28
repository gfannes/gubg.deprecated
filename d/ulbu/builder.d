module ulbu.builder;

import tango.io.File;

import gubg.puts;

import ulbu.element;
import ulbu.primitive;

class Builder
{
    this ()
	{
	    mRoot = new Root;
	}

    void build(char[] dirName, char[] fileName)
        {
	    // Create the element for compilation
            auto el = Element.createFrom(dirName, fileName);
	    mRoot.add(el);
	    // Add the system elements
	    addSystemElements();
	    // Set the locations into the elements
	    mRoot.setLocation();
	    // Resolve the unknown bodies
	    Body[char[]] fn2Body;
	    mRoot.collectBodies(fn2Body);
	    mRoot.resolveBodies(fn2Body);
        }

    void compile(char[] fileName)
        {
            scope fo = new File(fileName);
            fo.write(".section .data
startMessage:
	.ascii \"Starting...\\n\"
endMessage:
	.ascii \"End.\\n\"
.section .bss
.globl _start
.section .text

_start:
	movl $12, %edx		# Write start message
	movl $startMessage, %ecx
	movl $1, %ebx
	movl $4, %eax
	int $0x80

	movl $5, %edx		# Write end message
	movl $endMessage, %ecx
	movl $1, %ebx
	movl $4, %eax
	int $0x80
	
	movl $0, %ebx		# Exit
	movl $1, %eax
	int $0x80

");
            puts("OK");
            bool write2File(Element element)
            {
                puts("write2File for {}", element);
                if (!element.isRoot && element.isFunction)
                {
                    fo.append("_" ~ element.fullName ~ ":\n");
                }
                return true;
            }
            mRoot.depthFirst(&write2File);
        }

    void print()
	{
	    mRoot.print();
	}

private:
    void addSystemElements()
	{
	    mRoot.add(new Element(new Attributes([]), new Name("int"), new Integer));
	    mRoot.add(new Element(new Attributes([]), new Name("byte"), new Byte));
	}

    Root mRoot;
}

version (Test)
{
//     import tango.sys.Process: Process;

    void main()
    {
        auto builder = new Builder;

        builder.build("test", "test.ulbu");
	builder.print();
        puts("Before compilation");
        builder.compile("test.s");
        
//         Process.execute("rm test.o a.out");
//         Process.execute("as -o test.o test.s");
//         Process.execute("ld test.o");
//         Process.execute("./a.out");
    }
}
