
pub struct Cartouche
{
    tag: ~str,
    lines: ~[~str],
}

impl Cartouche
{
    pub fn new() -> Cartouche
    {
        Cartouche{tag: ~"<tag>", lines: ~[~"line0", ~"line1"]}
    }
    pub fn tag(self, tag: ~str) -> Cartouche
    {
        Cartouche{tag: tag, lines: self.lines}
    }
    pub fn print(&self)
    {
        println(self.tag);
        for line in self.lines.iter()
        {
            println!("\t{}", *line);
        }
    }
}
