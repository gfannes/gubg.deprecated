module gubg.random;

import tango.math.Random;

real uniform()
{
    return Random.shared.next/cast(real)uint.max;
}

real uniform(real min, real max)
{
    return min + uniform()*(max-min);
}
