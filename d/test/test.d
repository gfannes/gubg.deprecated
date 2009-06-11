module test;

import gubg.puts;

interface I
{
    void a();
}

interface ID: I
{
    void d();
}

class B: I
{
    void a()
    {
	puts("B.a()");
    }
}

class D: B, ID
{
    void d()
    {
	puts("D.d()");
    }
}

void compare(Object lhs, Object rhs)
{
    puts("compare = {}", lhs is rhs);
}

void main()
{
    D d = new D;
    I i = d;
    ID id = d;
    B b = d;
    I ib = b;
    puts("d = {}", cast(void*)d);
    puts("i = {}", cast(void*)i);
    puts("id = {}", cast(void*)id);
    puts("b = {}", cast(void*)b);
    puts("ib = {}", cast(void*)ib);
//     puts("d == i = {}", d==i);
//     puts("d == id = {}", d==id);
//     puts("d == b = {}", d==b);
//     puts("d == ib = {}", d==ib);
    puts("d is i = {}", d is i);
    puts("d is id = {}", d is id);
    puts("d is b = {}", d is b);
    puts("d is ib = {}", d is ib);
    puts("i is id = {}", i is id);
    puts("i is b = {}", i is b);
    puts("i is ib = {}", i is ib);
    compare(d, i);
    compare(d, id);
    compare(cast(I)d, b);
    compare(d, ib);
    compare(i, id);
    compare(i, b);
    compare(i, ib);
}
