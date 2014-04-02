//#[path="../cartouche.rs"]
//mod cartouche;
//extern mod cartouche;
extern mod gubg;

use gubg::cartouche::Cartouche;

fn main()
{
    println!("cartouche_tests");
    let mut c = Cartouche::new().tag(~"mytag");
    c.print();
}
