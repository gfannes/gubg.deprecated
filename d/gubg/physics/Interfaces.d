module gubg.physics.Interfaces;

interface IItem
{
    void update(real dT);
    void addForce(real[] force);
    void print();
    real[] location();
    void location(real[] loc);
}

interface IForce
{
    void apply(real dT);
}
