module gubg.physics.ball;

import gubg.physics.newton;
import gubg.physics.interfaces;

import gubg.puts;

class Ball(int dim): Newton!(dim)
{
    this (real mass, real radius)
	{
	    super(mass);
	    _radius = radius;
	    _charge = 0.0;
	}

    real radius(){return _radius;}
    real charge(){return _charge;}

    void print()
    {
	puts("Ball {} at location {}", cast(void*)(this), location);
    }
private:
    real _radius;
    real _charge;
}

version (UnitTest)
{
    void main()
    {
	auto ball = new Ball!(2)(1.0, 1.0);
    }
}
