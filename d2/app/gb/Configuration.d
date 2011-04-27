module Configuration;

import gubg.JSON;
import std.file;
import std.path;
import std.stdio;

bool verbose__ = false;

//Load the configuration file <filename>, optionally by stepping down to the root
//This file should be a JSON object (hash) of strings or arrays of strings
class Configuration
{
    this(string filename, string path = null, bool stepDown = false)
    {
        //Whatever happens, make sure we set the current working directory back to its original setting
        string origPath = getcwd();
        scope(exit) chdir(origPath);

        //Make sure we start with path == current working directory
        if (path)
            chdir(path);
        else
            path = getcwd();

        //Depending on stepDown, we either scan for filename in all parent directories
        //Or, we only look in path
        if (stepDown)
        {
            string prevPath;
            while (prevPath != path)
            {
                chdir(path);
                if (exists(filename))
                {
                    filepath_ = join(path, filename);
                    writefln("I found \"%s\"", filepath_);
                    break;
                }
                prevPath = path;
                path = dirname(path);
            }
        } else if (exists(filename))
            filepath_ = join(path, filename);

        //If we found the configuration file, parse it
        if (filepath_)
            parseConfigFile_();
    }
    bool isValid()
    {
        if (!jsonIsParsed_) return false;
        return true;
    }
    string filepath() const {return filepath_;}

    bool get(out string value, string key)
    {
        if (!isValid()) return false;
        return gubg.JSON.get(json_, key, value);
    }
    bool get(out string[] values, string key)
    {
        if (!isValid()) return false;
        try
        {
            foreach (value; lookup(json_, key))
                values ~= value;
        } catch (JSONException e)
        {
            return false;
        }
        return true;
    }

    private:
    string filepath_;
    JSONValue json_;
    bool jsonIsParsed_;

    void parseConfigFile_()
    {
        if (!filepath_)
            return;

        if (verbose__)
        {
            writefln("Parsing \"%s\"", filepath_);
            writefln("Content: %s", cast(char[])read(filepath_));
        }
        json_ = parseJSON(cast(char[])read(filepath_));
        jsonIsParsed_ = true;
    }
}

version(UnitTest)
{
    void main()
    {
        auto conf = new Configuration("gb.json", "/home/gfannes/gubg/d2/test/l1/l2");
        string executableName;
        writefln("var executableName %s", conf.get("executableName", executableName));
    }
}
