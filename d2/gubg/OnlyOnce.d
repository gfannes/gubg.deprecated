module gubg.OnlyOnce;

//A small struct meant to keep track of things that should happen only the first time
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
    bool opCall()
    {
        return firstTime();
    }

    void reset(){notFirstTime_ = false;}

    private:
        //This variable will get initialized to false
        bool notFirstTime_;
}

struct FewTimes
{
    this (uint nrTimes)
    {
        nrTimes_ = nrTimes;
        reset();
    }
    bool isActive()
    {
        if (current_ < nrTimes_)
        {
            ++current_;
            return true;
        }
        return false;
    }
    void reset()
    {
        current_ = 0;
    }
    private:
    uint nrTimes_;
    uint current_;
}

version (UnitTest)
{
    import std.stdio;
    void main()
    {
        OnlyOnce oo;
        assert(oo.firstTime());
        assert(!oo.firstTime());
        assert(!oo.firstTime());
        oo.reset();
        assert(oo());
        assert(!oo());
        writeln("Everything went OK");
    }
}
