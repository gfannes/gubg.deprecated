#include "xml.hpp"
#include <iostream>
using namespace gubg;
using namespace std;

int main()
{
    xml::Element html("html");
    html("head");
    auto &body = html("body");
    body.option("class", "brol").option("testEscaping", "&<>\'\"");
    body << "Hello HTML";
    body("br");
    body("br").option("testEscaping", "&<>\'\"");
    auto &br = body("br");
    body << "Second line" << "This is another content child, but should show on the same line in a browser";

    body << 123 << 456;

    body << "<tag>blabla &amp; </tag>";

    cout << html;
    return 0;
}
