module gubg.physics.Newton;

import gubg.physics.Interfaces;

import gubg.Array;
import gubg.Vector;

class Newton(int dim): IItem
{
    this(real mass)
	{
	    _mass = mass;
	    setSame!(real)(_location, 0.0);
	    setSame!(real)(_speed, 0.0);
	    setSame!(real)(_force, 0.0);
	}

    void addForce(real[] force)
	{
	    _force.add(force);
	}

    void update(real dT)
	{
	    // Compute the acceleration in _force, this variable will be cleared at the end anyway
	    _force.multiplyWith(1.0/_mass);

	    // Update the speed
	    _speed.add(_force, dT);

	    // Update the location
	    _location.add(_speed, dT);

	    // Clear _force for the next iteration
	    setSame!(real)(_force, 0.0);
	}

    real[] location(){return _location;}
    void location(real[] loc){_location[] = loc[];}
    real[] speed(){return _speed;}
    void speed(real[] s){_speed[] = s[];}

    void print()
    {
	puts("Newton {} at location {}", cast(void*)(this), location);
    }
    
private:
    real _mass;
    real[dim] _location;
    real[dim] _speed;
    real[dim] _force;
}

version (UnitTest)
{
    import gubg.Timer;
    import tango.core.Thread;
    import gubg.Puts;

    void main()
    {
	auto newton1 = new Newton!(2)(1.0);
	auto newton2 = new Newton!(2)(2.0);
	newton2.speed = [1.0, 1.0];

	auto timer = new Timer;
	for (uint i = 0; i < 1000; ++i)
	{
	    Thread.sleep(0.1);
	    real dT = timer.difference;
	    newton1.addForce([1.0, 1.0]);
	    newton2.addForce([1.0, 1.0]);
	    newton1.update(dT);
	    newton2.update(dT);
	    puts("n1 = {}, n2 = {}", newton1.location, newton2.location);
	}
    }
}
