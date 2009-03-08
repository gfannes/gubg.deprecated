module rinle.dir;

import tango.io.FilePath;
import tango.io.FileSystem;
import tango.util.PathUtil;
import tango.io.FileScan;
import tango.text.Util;

import rinle.globals;
import rinle.interfaces;
import rinle.fsnode;
import rinle.file;
import gubg.puts;

class Dir: FSNode
{
    this(char[] name){super(name);}

    static Dir root(ref Dir here, char[] path)
    {
	auto fp = FilePath(FilePath.stripped(normalize(FileSystem.toAbsolute(path))));

        // Create the path until the root
	Dir d;
        here = null;
 	while (fp.file.length > 0)
 	{
 	    auto parent = new Dir(fp.file);
 	    fp.pop;
            if (d !is null)
            {
                d.parent = parent;
                parent.addChild(d);
            } else
                here = parent;
            d = parent;
 	}

        // Add the root
        auto root = new Dir("");
        if (d !is null)
        {
            d.parent = root;
            root.addChild(d);
        } else
            here = root;
	return root;
    }

    void expand()
    {
        auto fp = FilePath(path());
        foreach (fph; fp.toList)
        {
            char[] str = fph.file;
            if (str[0] != '.' && !hasChild(str))
            {
                if (fph.isFolder)
                    addChild(new Dir(str));
                else
                    addChild(new File(str));
            }
        }
	sortChilds(delegate bool(INode lhs, INode rhs)
	     {
		 FSNode llhs = cast(FSNode)lhs;
		 FSNode rrhs = cast(FSNode)rhs;
		 if (llhs !is null && rrhs !is null)
		     return llhs.name <= rrhs.name;
		 else if (rrhs !is null)
		     return false;
		 else
		     return true;
	     });
    }

    void createChild(inout INode res, uint ix)
    {
	char[] name = env.askString("Name of file or directory to create: ");
	if (name.length > 0 && name[0] != '.' && !contains(name, '/'))
	{
	    auto fp = FilePath(path() ~ "/" ~ name);
	    if (!fp.exists)
	    {
		if (contains(name, '.'))
		{
		    puts("Creating file {}", name);
		    fp.createFile;
		    res = new File(name);
		} else
		{
		    puts("Creating dir {}", name);
		    fp.createFolder;
		    res = new Dir(name);
		}
	    }
	}
    }

    bool hasChild(char[] str)
    {
        bool found = false;
        eachChild(delegate bool(INode n)
                  {
                      FSNode f = cast(FSNode)n;
                      if (f !is null)
                      {
                          if (f.name == str)
                          {
                              found = true;
                              return false;
                          }
                      }
                      return true;
                  });
        return found;
    }
}

version (UnitTest)
{
    import rinle.sink;
    void main()
    {
        Dir root, here;
        root = Dir.root(here, "../");        
        puts("path = {}", here.path);
	here.expand();

	auto sink = new Sink;
	here.render(sink);
	char[] str;
	sink.showAsTree();
	sink.collect(str);
	puts("{}", str);
    }
}