module gubg.physics.world;

import gubg.physics.interfaces;

import gubg.timer;
import gubg.array;
import gubg.random;

import tango.time.Clock;

import gubg.puts;

class World
{
    this ()
	{
	    _timer = new Timer;
	}

    IItem add(IItem item)
	{
	    append(_items, item);
	    return item;
	}
    IForce add(IForce force)
	{
	    append(_forces, force);
	    return force;
	}

    void start()
	{
	    _timer.start;
	}
    void update()
	{
	    real dT = _timer.difference;

	    // Compute and apply all the forces
	    foreach (force; _forces)
		force.apply(dT);

	    // Update all the items
	    foreach (item; _items)
		item.update(dT);
	}

    void print()
	{
	    puts("\nTime {}", Clock.now.ticks);
	    foreach (item; _items)
		item.print;
	}

private:
    IItem[] _items;
    IForce[] _forces;
    Timer _timer;
}

version (UnitTest)
{
    import gubg.physics.ball;
    import gubg.physics.spring;

    import gubg.graphics.visu;
    import gubg.puts;
    import gubg.vector;

    import tango.core.Thread;
    import tango.math.Math;

    void main()
    {
	Visu visu = new Visu(640, 480);
	Drawable[IItem] drawablePerItem;
	Drawable[Spring] drawablePerSpring;

	// Create the world
	const uint dim = 2;
	auto world = new World;

	// Create the balls
	Ball!(dim)[] balls;
	balls.length = 40;
	foreach (ix, inout ball; balls)
	{
	    ball = new Ball!(dim)(0.1, 0.03);
	    world.add(ball);
	    real angle = 2*PI*ix/balls.length;
	    const real factor = 0.8;
	    ball.location([factor*cos(angle), factor*sin(angle)]);
	    drawablePerItem[ball] = visu.addCircle(ball.location, ball.radius);
	}

	// Create the springs between them
	real[] fracs = [0.05, 0.15, 0.5, 0.85, 0.95];
	uint[] ixs;
	ixs.length = fracs.length;
	foreach (i, inout ix; ixs)
	    ix = cast(uint)(balls.length*fracs[i]);
	foreach (i, from; balls)
	{
	    foreach (offset; ixs)
	    {
		auto ix = i + offset;
		if (ix < balls.length)
		{
		    auto to = balls[ix];
		    real dist = distance(from.location, to.location);
		    auto spring = new Spring([from, to], dist, 1.0, 10.0);
		    world.add(spring);
//		    drawablePerSpring[spring] = visu.addLine(from.location, to.location);
		}
		visu.addLine(from.location, balls[(i+1)%$].location);
	    }
	    auto loc = from.location.dup;
	    add(loc, uniform(-0.1, 0.1, dim));
	    from.location(loc);
	}

	// Simulate the world
	visu.show(true);
	world.start;
	for (uint i = 0; i < 200; ++i)
	{
//	    world.print;
	    Thread.sleep(0.01);
	    world.update();
	}
	visu.stop;
    }
}
