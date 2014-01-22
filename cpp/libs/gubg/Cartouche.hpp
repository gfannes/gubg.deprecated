#ifndef HEADER_gubg_Cartouche_hpp_ALREADY_INCLUDED
#define HEADER_gubg_Cartouche_hpp_ALREADY_INCLUDED

//A Cartouche is a piece of text/code, typically embedded in the comments of some other code
//
//The start of a Cartouche is indicated as:
//^<pre>!@#<tag>$
//<pre>: can be any string, but it has to start from the start of a line
//<tag>: the tag of the cartouche, which can e.g., be the programming language it is written in
//       the tag runs till the end of the line
//
//The content of the cartouche is added to the line(s) after the start line:
//^<pre><content>$
//<pre>: has to be the exact string as was detected in the start line
//
//The cartouche extends until <pre> is not found anymore, or until the end-of-file
//
//An example:
//!@#lua
//print("You can use !@# here without problems")
//print("This is still part of the cartouche")
 //The leading space stops the cartouche

#include <string>
#include <vector>

namespace gubg
{
    struct Cartouche
    {
        std::string tag;
        std::string content;

        typedef std::vector<Cartouche> Cartouches;
        static Cartouches parse(const std::string &content);
    };
}

#endif
