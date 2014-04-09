use std::io::stdio::println;

pub struct Cartouche
{
    pub tag: ~str,
    pub lines: ~[~str],
}

impl Cartouche
{
    pub fn new() -> Cartouche
    {
        Cartouche{tag: ~"", lines: ~[]}
    }
    pub fn parse(content: &str) -> ~[~Cartouche]
    {
        println!("Content: {}", content);
        let mut res = ~[];
        res
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
