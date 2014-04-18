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

pub struct Cartouches<'a>
{
	content: ~str,
	pub cartouches: ~[&'a str],
}

impl<'a> Cartouches<'a>
{
	pub fn from_str(content: ~str) -> Cartouches
	{
		let mut res = Cartouches{content: content, cartouches: ~[]};
		let mut ix = 0;
		for line in res.content.lines_any()
		{
			ix += 1;
			println!("{}: {}", ix, line);
			res.cartouches.push(line);
		}
		Cartouches{content: ~"bla", cartouches: ~[]}
	}
}
