#include "gubg/Manuscript.hpp"
#include "gubg/file/Filesystem.hpp"
using namespace std;

#define GUBG_MODULE_ "Manuscript"
#include "gubg/log/begin.hpp"
namespace gubg { 

    namespace priv { 
        string read(const gubg::file::File &file)
        {
            string content;
            gubg::file::read(content, file);
            return content;
        }

        bool findTag(gubg::Strange &tag, std::string &pre, gubg::Strange line)
        {
            static string start("!@#");
            static string stop("#@!");

            string tmpPre;
            if (!line.popUntil(tmpPre, start))
                //The tag start sequence was not found
                return false;
            gubg::Strange tmpTag;
            if (!line.popUntil(tmpTag, stop))
                //The tag stop sequence was not found
                return false;
            if (!line.empty())
                //There are extra characters after stop, which is not allowed for a tag line
                return false;
            tag = tmpTag;
            pre.swap(tmpPre);
            return true;
        }

        Paragraphs parse(const string &content)
        {
            S();
            Paragraphs paras;
            Paragraph para;
            gubg::Strange strange(content);
            gubg::Strange line, tag;
            std::string pre;

            //Iterate over all the lines
            while (strange.popLine(line))
            {
                S();L(line);
                if (findTag(tag, pre, line))
                {
                    L("This is a tag line (" << STREAM(tag, pre) << "), we have to append the current paragraph and start a new one");
                    if (!para.empty())
                        paras.push_back(para);
                    para = Paragraph(tag);
                    continue;
                }
                if (line.popString(pre))
                {
                    L("This line starts with the same pre (" << pre << ") as the current para; it is part of it");
                    para.addLine(line);
                    continue;
                }
                L("The current pre (" << pre << ") was not found: we have to start a new default para");
                if (!para.empty())
                    paras.push_back(para);
                pre.clear();
                para = Paragraph();
                para.addLine(line);
            }
            //Append the current paragraph, if present
            if (!para.empty())
                paras.push_back(para);
            return paras;
        }
    } 

    //Manuscript
    Manuscript::Manuscript(const string &content):
        content_(content), paragraphs_(priv::parse(content_))
    {
    }
    Manuscript::Manuscript(const gubg::file::File &file):
        content_(priv::read(file)), paragraphs_(priv::parse(content_))
    {
    }
    Paragraphs::iterator begin(Manuscript &m)
    {
        return m.paragraphs_.begin();
    }
    Paragraphs::iterator end(Manuscript &m)
    {
        return m.paragraphs_.end();
    }

} 
#include "gubg/log/end.hpp"
