module gubg.JSON;

import gubg.Format;
public import std.json;
import std.stdio;

//Provides easy lookup of a specific JSON object or array to be used for iteration
JSONIterator lookup(JSONValue json, string[] path...)
{
    if (path.length > 0)
    {
        if (JSON_TYPE.OBJECT != json.type)
           throw new Exception("json is not an object, I cannot lookup a path");
        if (!(path[0] in json.object))
            throw new Exception(Format.immediate("Element with key %s could not be found", path[0]));
        return lookup(json.object[path[0]], path[1 .. $]);
    }
    return new JSONIterator(json);
}
//Provides easy iteration over a JSON object or array
JSONIterator iterate(JSONValue json)
{
    return new JSONIterator(json);
}
//Converts a JSONValue to a string, or throw if not possible
string toString(JSONValue json)
{
    if (JSON_TYPE.STRING != json.type)
        throw new Exception("json is not a string");
    return json.str;
}
//Retrieves a string value from an object
string get(JSONValue json, string key, out string value)
{
    if (JSON_TYPE.OBJECT != json.type)
        throw new Exception("json is not an object");
    if (!(key in json.object))
        throw new Exception(Format.immediate("Element with key %s could not be found", key));
    if (JSON_TYPE.STRING != json.object[key].type)
        throw new Exception("json.object[key] is not an object");
    return json.object[key].str;
}

//Iterator used to iterator over JSON objects or arrays
class JSONIterator
{
    this(JSONValue json)
    {
        json_ = json;
    }

    //Iterates over the key-value pairs of an object
    int opApply(int delegate(ref string, ref JSONValue) dg)
    {
        if (JSON_TYPE.OBJECT != json_.type)
            return 1;

        foreach (k, v; json_.object)
        {
            if (dg(k, v))
                return 1;
        }
        return 0;
    }
    //Iterates over the elements of an array, assuming they are all strings
    int opApply(int delegate(ref string) dg)
    {
        if (JSON_TYPE.ARRAY != json_.type)
            return 1;

        foreach (v; json_.array)
        {
            if (JSON_TYPE.STRING != v.type)
                return 1;
            if (dg(v.str))
                return 1;
        }
        return 0;
    }

    private:
    JSONValue json_;
}

version(UnitTest)
{
    auto jsonString = "{ \"binaryType\": \"executable\", \"files\": [\"test.d\"], \"includePaths\": [\"$HOME/gubg/d2\"] }";
    void main()
    {
        auto json = parseJSON(jsonString);

        foreach (name, js; lookup(json))
        {
            writefln("name = %s", name);
            switch (name)
            {
                case "includePaths":
                    foreach (includePath; iterate(js))
                    writefln("incl path %s", includePath);
                break;
                case "binaryType":
                    writefln("Binary type = %s", toString(js));
                break;
                case "files":
                    foreach (file; iterate(js))
                    writefln("file %s", file);
                break;
                default: break;
            }
        }
        foreach (includePath; lookup(json, "includePaths"))
        {
            writefln("includePath = %s", includePath);
        }
    }
}
