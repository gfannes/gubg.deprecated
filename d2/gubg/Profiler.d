module gubg.Profiler;

import gubg.Timer;
import gubg.Format;
import std.range;

import std.stdio;

//Not yet multithreaded, access to profilerByName__ should be protected

class Profiler
{
    this (string name)
    {
        timer_ = Timer(ResetType.NoAuto);
        name_ = name;
    }
    scope class Task
    {
        this (string name)
        {
            timer_ = Timer(ResetType.NoAuto);
            name_ = name;
            start_();
        }
        ~this ()
        {
            stop_();
        }
        void change(string newName)
        {
            stop_();
            name_ = newName;
            start_();
        }
        private:
        void start_()
        {
            if (!lockingTask_.empty)
                throw new Exception(Format.immediate("Profiler is already locked by %s, but %s tried to lock it again", lockingTask_, name_));
            lockingTask_ = name_;
            timer_.reset();
        }
        void stop_()
        {
            if (lockingTask_.empty)
                throw new Exception(Format.immediate("Profiler is not locked, but %s tried to unlock it", name_));
            if (name_ != lockingTask_)
                throw new Exception(Format.immediate("Profiler is locked by %s but %s tried to unlock it", lockingTask_, name_));
            elapsePerTask_[name_] = elapsePerTask_.get(name_, 0.0) + timer_.difference;
            lockingTask_ = "";
        }
        Timer timer_;
        string name_;
    }

    void reset()
    {
        timer_.reset();
        real[string] emptyHash;
        elapsePerTask_ = emptyHash;
    }

    string toString()
    {
        real totalElapse = timer_.difference;
        //Compute how much elapse we can explain
        real explainedElapse = 0.0;
        foreach (task, elapse; elapsePerTask_)
            explainedElapse += elapse;
        real unknownElapse = totalElapse - explainedElapse;

        Format format;
        format.delimiter = "\n";
        format("Total elapse: %s", totalElapse);
        format("Unknown elapse: %s (frac: %s)", unknownElapse, unknownElapse/totalElapse);
        foreach (task, elapse; elapsePerTask_)
            format("%s: %s (frac: %s)", task, elapse, elapse/totalElapse);
        return format.toString;
    }
    
    private:
    Timer timer_;
    string name_;
    string lockingTask_;
    struct Elapse
    {
        this (string n, real e){name = n; elapse = e;}
        string name;
        real elapse;
    }
    real[string] elapsePerTask_;
}

version (UnitTest)
{
    import std.stdio;
    import core.thread;
    void main()
    {
        foreach (i; 0 .. 1)
        {
            scope p = new Profiler("base");

            //Task1
            {
                scope task = p.new Task("task1");
                Thread.sleep(100000);
            }

            //Task2
            {
                scope task = p.new Task("task2");
                Thread.sleep(200000);
            }

            writefln("%s", p.toString);
        }
    }
}
