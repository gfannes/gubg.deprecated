module gubg.coordinate;

import tango.math.Math;

import gubg.puts;

// Coordinate transformations (translation and rotation)
class Transformation
{
    // newOrigCo: coordinates of new origin in old coordinates
    // newOneOne: coordinates of new (1,1) in old coordinates (rotation is not yet taken into account, fix this)
    this(real[] newOrigCo, real[] newOneOne, real angle = 0.0)
	{
	    mTranslation = new Translation(newOrigCo);
	    real[] scale;
	    scale.length = newOrigCo.length;
	    foreach (ix, o; newOrigCo)
		scale[ix] = newOneOne[ix] - o;
	    mScale = new Scale(scale);
	    mRotation = new Rotation(angle);
	}

    bool transformCoN2O(out real[] oco, in real[] nco)
	{
	    return mRotation.rotateN2O(oco, nco) && mScale.scaleN2O(oco, oco) && mTranslation.translateN2O(oco, oco);
	}

    bool transformCoO2N(out real[] nco, in real[] oco)
	{
	    return mTranslation.translateO2N(nco, oco) && mRotation.rotateO2N(nco, nco);
	}

    bool transformLN2O(out real[] oco, in real[] nco)
	{
	    return mRotation.rotateN2O(oco, nco) && mScale.scaleN2O(oco, oco);
	}

    class Translation
    {
	this(real[] newOrigCo)
	    {
		mNewOrigCo = newOrigCo;
	    }
	bool translateN2O(out real[] oco, in real[] nco)
	    {
		if (mNewOrigCo.length != nco.length)
		{
		    err("Length of coordinate vector and translation vector should be the same");
		    return false;
		}
		oco.length = nco.length;
		foreach(ix, v; mNewOrigCo)
		    oco[ix] = nco[ix] + v;
		return true;
	    }
	bool translateO2N(out real[] nco, in real[] oco)
	    {
		if (mNewOrigCo.length != oco.length)
		{
		    err("Length of coordinate vector and translation vector should be the same");
		    return false;
		}
		nco.length = oco.length;
		foreach(ix, v; mNewOrigCo)
		    nco[ix] = oco[ix] - v;
		return true;
	    }
    private:
	real[] mNewOrigCo;
    }

    class Scale
    {
	this(real[] newScales)
	    {
		mNewScales = newScales;
	    }
	bool scaleN2O(out real[] oco, in real[] nco)
	    {
		if (mNewScales.length != nco.length)
		{
		    err("Length of coordinate vector and scale vector should be the same");
		    return false;
		}
		oco.length = nco.length;
		foreach(ix, v; mNewScales)
		    oco[ix] = nco[ix] * v;
		return true;
	    }
	bool scaleO2N(out real[] nco, in real[] oco)
	    {
		if (mNewScales.length != oco.length)
		{
		    err("Length of coordinate vector and scale vector should be the same");
		    return false;
		}
		nco.length = oco.length;
		foreach(ix, v; mNewScales)
		    nco[ix] = oco[ix] / v;
		return true;
	    }
    private:
	real[] mNewScales;
    }

    class Rotation
    {
	this(real angle)
	    {
		setAngle(angle);
	    }

	void setAngle(real angle)
	    {
		mAngle = angle;
		mSin = sin(angle);
		mCos = cos(angle);
	    }

	bool rotateN2O(out real[] oco, in real[] nco)
	    {
		if (nco.length != 2)
		{
		    err("Length of coordinate vector for rotation should be the 2");
		    return false;
		}
		oco.length = 2;
		real x = nco[0];
		oco[0] = x*mCos - nco[1]*mSin;
		oco[1] = x*mSin + nco[1]*mCos;
		return true;
	    }
	bool rotateO2N(out real[] nco, in real[] oco)
	    {
		if (oco.length != 2)
		{
		    err("Length of coordinate vector for rotation should be the 2");
		    return false;
		}
		nco.length = 2;
		real x = oco[0];
		nco[0] =  x*mCos + oco[1]*mSin;
		nco[1] = -x*mSin + oco[1]*mCos;
		return true;
	    }
    private:
	real mAngle;
	real mSin;
	real mCos;
    }

private:
    Translation mTranslation;
    Scale mScale;
    Rotation mRotation;
}

version(UnitTest)
{
    void main()
    {
	auto transf = new Transformation([1,2], [2,3], 0.2);

	{
	    puts("New -> old -> new");
	    real[] co = [1,0], o;
	    puts("{}",co);
	    transf.transformCoN2O(o, co);
	    puts("{}",co);
	    transf.transformCoO2N(o, co);
	    puts("{}",co);
	}

	{
	    puts("Old -> new -> old");
	    real[] co = [1,0], o;
	    puts("{}",co);
	    transf.transformCoO2N(o, co);
	    puts("{}",co);
	    transf.transformCoN2O(o, co);
	    puts("{}",co);
	}
    }
}