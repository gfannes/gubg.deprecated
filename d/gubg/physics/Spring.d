module gubg.physics.Spring;

import gubg.physics.Interfaces;

import gubg.Vector;
import gubg.Puts;

class Spring: IForce
{
    this (IItem[2] items, real length, real k = 1.0, real damp = 0.0)
    {
	this(items, [length, length], [k, k], [damp, damp]);
    }
    // lengths: stretch and shrink lengths of the spring
    // ks: stretch and shrink K-values of the spring
    // damps: stretch and shrink damping coefficients of the spring
    this (IItem[2] items, real[2] lengths, real[2] ks, real[2] damps)
    {
	if (items[0] is items[1])
	    throw new Exception("ERROR::You cannot add a spring between two nodes that are the same.");
	_items[] = items[];
	_lengths[] = lengths[];
	_ks[] = ks[];
	_damps[] = damps[];
	_prevDT = 0.0;
    }

    void apply(real dT)
    {
	// Compute the spring force
	auto dir = direction(_items[0].location, _items[1].location);
	auto springLength = l2Norm(dir);
	if (springLength < 0.000000001)
	    puts("Warning from Spring.apply():Two items ({}, {}) are rather close: {}", cast(void*)(_items[0]), cast(void*)(_items[1]), springLength);
	real forceFactor = 0.0;
	if (springLength < _lengths[1])
	    forceFactor = _ks[1]*(_lengths[1] - springLength)/springLength;
	else if (springLength > _lengths[0])
	    forceFactor = _ks[0]*(_lengths[0] - springLength)/springLength;
	real[] force = dir.dup;
	force.multiplyWith(forceFactor);

	// Add the damping force
	if (_prevDT > 0)
	{
	    real speed = (springLength - _prevLength)/_prevDT;
	    if (speed > 0)
		forceFactor = -_damps[0]*speed/springLength;
	    else
		forceFactor = -_damps[1]*speed/springLength;
	    puts("forceFactor = {}",forceFactor);
	    force.add(dir, forceFactor);
	}
	_prevLength = springLength;
	_prevDT = dT;

	// Add the force to both ends
	_items[1].addForce(force);
	invertSign(force);
	_items[0].addForce(force);
    }

private:
    IItem[2] _items;
    real[2] _lengths;
    real[2] _ks;
    real[2] _damps;
    real _prevDT;
    real _prevLength;
}

version (UnitTest)
{
    import gubg.physics.Ball;

    void main()
    {
	const uint dim = 2;
	auto spring = new Spring([new Ball!(dim)(1.0, 1.0), new Ball!(dim)(1.0, 1.0)], [1, 1], [1, 1], [1, 1]);
    }
}
