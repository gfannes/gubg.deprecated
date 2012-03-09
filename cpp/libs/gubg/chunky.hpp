#ifndef gubg_chunky_hpp
#define gubg_chunky_hpp

#include <vector>
#include <string>
#include <ostream>

namespace gubg
{
    class Chunky;
}
std::ostream &operator<<(std::ostream &, const gubg::Chunky &);

namespace gubg
{
    using namespace std;
    class Chunky
    {
        public:
            struct iterator
            {
                size_t ix_;
                string::iterator it_;
                Chunky *outer_;

                bool operator==(const iterator &) const;
                bool operator!=(const iterator &rhs) const {return !(*this == rhs);}
                iterator &operator++();
                char operator*() const;
            };

            Chunky(size_t size = 1024);

            void add(const string &);
            void add(char ch);

            size_t size() const;
            bool empty() const;

            void clear();

            iterator begin();
            iterator end();

        private:
            friend std::ostream &::operator<<(std::ostream &, const Chunky &);
            size_t size_;
            string chunk_;
            vector<string> chunks_;
    };
}

#endif
