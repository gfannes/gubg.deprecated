#ifndef HEADER_imui_WidgetId_hpp_ALREADY_INCLUDED
#define HEADER_imui_WidgetId_hpp_ALREADY_INCLUDED

#include <cstddef>
#include <ostream>

namespace imui { 

    class WidgetId
    {
        public:
            WidgetId();
            WidgetId(const char *file, size_t line, size_t ix);

            bool operator==(const WidgetId &) const;

            void clear() {*this = WidgetId{};}
            bool valid() const {return file_ != nullptr;}

            void stream(std::ostream &os) const;

        private:
            const char *file_ = nullptr;
            size_t line_ = 0;
            size_t ix_ = 0;
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
