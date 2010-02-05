module gubg.Logistic;

import tango.math.Math;

real logit(real p)
{
    return log(p/(1.0-p));
}

real logistic(real x)
{
    return 1.0/(1.0+exp(-x));
}

version (UnitTest)
{
    import gubg.Puts;
    void main()
    {
        for (real x = -5; x < 5; x += 0.1)
        {
            auto p = logistic(x);
            puts("logistic({}) = {}", x, p);
            puts("logit({}) = {}", p, logit(p));
        }
    }
}
