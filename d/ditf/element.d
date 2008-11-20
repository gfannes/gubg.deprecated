module ditf.element;

abstract class Element
{
    char[] name();
    Element[] variables();
    uint size()
        {
            uint s = 0;
            foreach (Element el; variables)
                s += el.size;
            return size;
        }
}
