//#[path="../cartouche.rs"]
//mod cartouche;
//extern mod cartouche;
extern crate gubg;

use gubg::cartouche::Cartouche;
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
    let tmp = fi.read_to_end().unwrap();
    let content: &str = std::str::from_utf8(tmp).unwrap();
    let cs = Cartouche::parse(content);
}
