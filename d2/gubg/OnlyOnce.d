module gubg.OnlyOnce;

//A small class meant to keep track of things that should happen only the first time
//Upon its first invocation, firstTime() will return true, next, it will always return false
struct OnlyOnce
{
    bool firstTime()
    {
        if (notFirstTime_)
            return false;
        notFirstTime_ = true;
        return true;
    }

    private:
        //This variable will get initialized to false
        bool notFirstTime_;
}

version (UnitTest)
{
    import std.stdio;
    void main()
    {
        OnlyOnce oo;
        assert(oo.firstTime);
        assert(!oo.firstTime);
        assert(!oo.firstTime);
        assert(!oo.firstTime);
        writeln("Everything went OK");
    }
}
