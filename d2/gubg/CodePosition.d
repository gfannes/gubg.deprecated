module gubg.CodePosition;

//Returns the position in the source code where a call to this function will continue
void *codePosition()
{
    void *res;
    asm
    {
        mov EAX, dword ptr [EBP+4];
        mov res[EBP], EAX;
    }
    return res;
}

version (UnitTest)
{
    import std.stdio;
    void main()
    {
        foreach (i; 0 .. 10)
        {
            writefln("Iteration %s", i);
            writefln("%s", codePosition());
            writefln("%s", codePosition());
            writefln("%s", codePosition());
        }
    }
}
