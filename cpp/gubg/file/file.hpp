#ifndef gubg_file_hpp
#define gubg_file_hpp

#include <string>
#include <fstream>

namespace file
{
    bool readContent(std::string &content, const std::string &filename)
    {
        //Open the file
        std::ifstream fi(filename, std::ifstream::binary);
        if (!fi.is_open())
        {
            std::cerr << "Could not open file " << filename << std::endl;
            return false;
        }

        //Get the file size
        auto begin = fi.tellg();
        fi.seekg(0, std::ios::end);
        auto end = fi.tellg();
        auto size = end - begin;

        //Allocate enough space to fit the file
        content.resize(size);

        //Read the file
        fi.seekg(0, std::ios::beg);
        fi.read(&content[0], size);
        fi.close();

        return true;
    }

};

#endif
