module gubg.physics.World;

import gubg.physics.Interfaces;

import gubg.Timer;
import gubg.Array;
import gubg.Random;

import tango.time.Clock;

import gubg.Puts;

class World
{
    this ()
    {
    }

    IItem add(IItem item)
    {
        append(items_, item);
        return item;
    }
    IForce add(IForce force)
    {
        append(forces_, force);
        return force;
    }

    void start()
    {
        timer_ = new Timer;
    }
    void update()
    {
        real dT = timer_.difference;

        // Compute and apply all the forces
        foreach (force; forces_)
            force.apply(dT);

        // Update all the items
        foreach (item; items_)
            item.update(dT);
    }

    void print()
    {
        puts("\nTime {}", Clock.now.ticks);
        foreach (item; items_)
            item.print;
    }

    private:
    IItem[] items_;
    IForce[] forces_;
    Timer timer_;
}

version (UnitTest)
{
    import gubg.physics.Ball;
    import gubg.physics.Spring;

    import gubg.graphics.Visu;
    import gubg.Puts;
    import gubg.Vector;

    import tango.core.Thread;
    import tango.math.Math;

    void main()
    {
        scope visu = Visu.create(640, 480, [-1.0, 1.0], [-1.0, 1.0]);
        scope of = new Factory;
        IDrawable[IItem] drawablePerItem;
        IDrawable[Spring] drawablePerSpring;

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
            drawablePerItem[ball] = visu.add(of.createCircle(ball.location, ball.radius));
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
                    auto spring = new Spring([from, to], dist, 0.001, 10.0);
                    world.add(spring);
                    //		    drawablePerSpring[spring] = visu.addLine(from.location, to.location);
                }
                of.createLine(from.location, balls[(i+1)%$].location);
            }
            auto loc = from.location.dup;
            add(loc, uniform(-0.1, 0.1, dim));
            from.location(loc);
        }

        // Simulate the world
        visu.show(true, null, 0.001);
        world.start;
        for (uint i = 0; !visu.isFinished && (i < 200); ++i)
        {
            //	    world.print;
            Thread.sleep(0.01);
            world.update();
        }
    }
}
