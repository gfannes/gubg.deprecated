module gubg.FileCache;

import gubg.Format;
import std.path;
import std.file;
import std.date;
import std.md5;
import std.string;
import std.array;

//Manages a file cache where files are stored using a unique name constructed from the file name and other attributes (FileInfo)
//The create()-function indicates if the file was found in the cache, and will create it using the passed delegate if not
class FileCache
{
    static class Exc: Exception
    {
        this(string msg){super(msg);}
    }
    this(string storagePath = null)
    {
        storagePath_ = storagePath;
        if (!storagePath_)
        {
            version (Posix) storagePath_ = "/tmp/FileCache";
            version (Windows) storagePath_ = "c:\\temp\\FileCache";
        }
        if (!exists(storagePath_))
            mkdirRecurse(storagePath_);
    }

    //The full, unique name of the file in the file cache
    string filepath(FileInfo fi)
    {
        return storagePath_.join(fi.filename);
    }

    //Returns true if creater was used to successfully create fi
    enum Result {AlreadyPresent, CreationOK, CreationFailed}
    Result create(FileInfo fi, bool delegate(FileInfo fi) creater)
    {
        //The filepath we have to create if it doesn't exist yet
        auto fp = filepath(fi);

        //Local functionality: returns true if fp is present and a file, false if it is absent and throw in all other cases
        bool fileExists_()
        {
            if (!exists(fp))
                return false;
            if (!isfile(fp))
                throw new Exc(Format.immediate("\"%s\" exists but is not a file.", fp));
            return true;
        }

        //Check if the file already exists
        if (fileExists_())
            return Result.AlreadyPresent;

        //Try to create the file and check if it is actually there
        if (!creater(fi))
            return Result.CreationFailed;
        if (!fileExists_())
            throw new Exc(Format.immediate("I should have created file \"%s\", but it is not there...", fp));

        return Result.CreationOK;
    }
    void clean()
    {
        rmdirRecurse(storagePath_);
    }

    private:
        string storagePath_;
}

//Holds a filename together with keyed attributes and constructs a unique name based on this filename and the attributes
struct FileInfo
{
    this(string filename)
    {
        filename_ = filename;
    }
    string filename()
    {
	    string res;
	    {
	    if (getExt(filename_))
		    res = Format.immediate("%s-%s.%s", getName(filename_), optionsDigest_(), getExt(filename_)).replace(sep, "_");
	    else
		    res = Format.immediate("%s.%s", filename_, optionsDigest_()).replace(sep, "_");
	    }
	    if (!std.path.altsep.empty)
	    {
		    res = res.replace(altsep, "_");
		    res = res.replace(":", "_");
	    }
	    return res;
    }
    string get(T)(string key) if (is(T == string))
    {
        return strings_[key];
    }
    string[] get(T)(string key) if (is(T == string[]))
    {
        if (key in arrays_)
            return arrays_[key];
        return [];
    }
    //Set keyed information (strings)
    void set(T)(string key, T value) if (is(T == string))
    {
        strings_[key] = value;
    }
    //Set keyed information (arrays of strings)
    void set(T)(string key, T value) if (is(T == string[]))
    {
        arrays_[key] = value;
    }
    //Add another string to a keyed array of strings
    void add(string key, string value)
    {
        string[]* v = key in arrays_;
        if (v)
            *v ~= value;
        else
            arrays_[key] = [value];
    }
    //Add a file for which modification should be tracked
    //Currently, only its timestamps are tracked, not the actual content nor its size
    void addFile(string filepath)
    {
        filepaths_ ~= filepath;
    }

    private:
    string filename_;
    string[string] strings_;
    string[][string] arrays_;
    string[] filepaths_;
    Format format;

    string optionsDigest_()
    {
        MD5_CTX context;
        context.start();
        //Filename itself
        context.update(filename_);
        //Keyed strings
        foreach (string key; strings_.keys.sort)
        {
            context.update(key);
            context.update(strings_[key]);
        }
        //Keyed arrays of strings
        foreach (string key; arrays_.keys.sort)
        {
            context.update(key);
            foreach (string value; arrays_[key].sort)
                context.update(value);
        }
        //Files: we only use creation and modification time information
        foreach (string filepath; filepaths_.sort)
        {
            d_time c, a, m;
            getTimes(filepath, c, a, m);
            context.update(Format.immediate("%d %d", c, m));
        }
        //Compute the digest value and return it as a string
        ubyte digest[16];
        context.finish(digest);
        return digestToString(digest);
    }
}

version (UnitTest)
{
    import gubg.Build;
    import std.stdio;
    void main()
    {
        string sourceFilepath = "FileCache.d";
        auto fc = new FileCache("/tmp/gb");
        auto fi = FileInfo("FileCache.o");
        fi.set("sourceFilepath", sourceFilepath);
        fi.addFile(sourceFilepath);
        fi.add("includePaths", "/home/gfannes/gubg/d2");
        fc.create(fi, delegate bool(FileInfo fi){
                scope compile = new Compile(fi.get!(string)("sourceFilepath"));
                compile.setObjectFilepath(fc.filepath(fi));
                foreach (includePath; fi.get!(string[])("includePaths"))
                compile.addIncludePath(includePath);
                return compile.execute();
                });
    }
}
