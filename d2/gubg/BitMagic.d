module gubg.BitMagic;

uint reverseBits(uint v)
{
    if (0 == v)
        return v;
    //Exchange bits pairwise
    v = ((v & 0x55555555) << 1)  | ((v & 0xaaaaaaaa) >> 1);
    //Exchange bits in groups of 2
    v = ((v & 0x33333333) << 2)  | ((v & 0xcccccccc) >> 2);
    //Exchange bits in groups of 4
    v = ((v & 0x0f0f0f0f) << 4)  | ((v & 0xf0f0f0f0) >> 4);
    //Exchange bits in groups of 8
    v = ((v & 0x00ff00ff) << 8)  | ((v & 0xff00ff00) >> 8);
    //Exchange bits in groups of 16
    v = ((v & 0x0000ffff) << 16) | ((v & 0xffff0000) >> 16);
    return v;
}

version (UnitTest)
{
    void main()
    {
        //tests for reverseBits()
        assert(0 == reverseBits(0));
        assert(0x80000000 == reverseBits(0x00000001));
        assert(0x40000000 == reverseBits(0x00000002));
        assert(0xc0000000 == reverseBits(0x00000003));
        assert(0x1e6a2c48 == reverseBits(0x12345678));
        foreach (uint i; 0 .. 1000)
        {
            auto r = reverseBits(i);
            auto rr = reverseBits(r);
            assert(rr == i);
        }
    }
}
