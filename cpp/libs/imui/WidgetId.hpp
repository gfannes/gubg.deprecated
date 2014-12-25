#ifndef HEADER_imui_WidgetId_hpp_ALREADY_INCLUDED
#define HEADER_imui_WidgetId_hpp_ALREADY_INCLUDED

#include <cstddef>
#include <ostream>

namespace imui { 

    class WidgetId
    {
        public:
            WidgetId();
            WidgetId(const char *file, int line, int ix);

            bool operator==(const WidgetId &) const;

            void clear() {*this = WidgetId{};}
            bool valid() const {return file_ != nullptr;}

            void stream(std::ostream &os) const;

        private:
            const char *file_ = nullptr;
            int line_ = 0;
            int ix_ = 0;
    };
#define IMUI_ID_IX(ix) WidgetId{__FILE__, __LINE__, ix}
#define IMUI_ID() IMUI_ID_IX(0)

    inline std::ostream &operator<<(std::ostream &os, const WidgetId &id)
    {
        id.stream(os);
        return os;
    }

} 

#endif
