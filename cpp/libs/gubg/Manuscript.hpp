#ifndef HEADER_gubg_Manuscript_hpp_ALREADY_INCLUDED
#define HEADER_gubg_Manuscript_hpp_ALREADY_INCLUDED

#include "gubg/file/File.hpp"
#include "gubg/Strange.hpp"
#include <vector>

namespace gubg { 

    class Paragraph
    {
        public:
            typedef gubg::Strange Tag;
            typedef std::vector<gubg::Strange> Lines;

            Paragraph() {}
            Paragraph(Tag tag): tag_(tag) {}

            bool empty() const {return tag_.empty() && lines_.empty();}

            Tag tag() const {return tag_;}
            Lines lines() const {return lines_;}

            void addLine(gubg::Strange line) {lines_.push_back(line);}

        private:
            Tag tag_;
            Lines lines_;
    };

    typedef std::vector<Paragraph> Paragraphs;

    class Manuscript;
    Paragraphs::iterator begin(Manuscript &);
    Paragraphs::iterator end(Manuscript &);
    class Manuscript
    {
        public:
            Manuscript(const std::string &content);
            Manuscript(const gubg::file::File &);

        private:
            const std::string content_;
            friend Paragraphs::iterator begin(Manuscript &);
            friend Paragraphs::iterator end(Manuscript &);
            Paragraphs paragraphs_;
    };

} 

#endif
