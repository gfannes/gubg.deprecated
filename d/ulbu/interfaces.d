module ulbu.interfaces;

interface

interface IBuilder
{
    IBuilder setLocation(char[] loc);
    IBuilder addLocation(char[] loc);
    Body resolveBody(char[] bodyName);
}
