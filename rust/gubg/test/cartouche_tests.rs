//#[path="../cartouche.rs"]
//mod cartouche;
//extern mod cartouche;
extern crate gubg;

use gubg::cartouche::Cartouche;
use gubg::cartouche::Cartouches;
use std::io::{File, BufferedReader};

fn main()
{
    println!("cartouche_tests");
    let mut c = Cartouche::new();
    c.tag = ~"tag";
    c.lines.push(~"line0");
    c.lines.push(~"line1");
    c.print();

    let cs = Cartouche::parse("oeuauoeaeu");

    let path = Path::new("cartouche.rs");
    let mut fi = File::open(&path);
    let content = fi.read_to_str().unwrap();
    let cs = Cartouche::parse(content);

	let css = Cartouches::from_str(content);
}
