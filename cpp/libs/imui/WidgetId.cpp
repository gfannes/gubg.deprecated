#include "imui/WidgetId.hpp"

namespace imui { 

    WidgetId::WidgetId()
    {
    }
    WidgetId::WidgetId(const char *file, size_t line, size_t ix): file_(file), line_(line), ix_(ix)
    {
    }
    bool WidgetId::operator==(const WidgetId &rhs) const
    {
        if (!valid())
            return !rhs.valid();
        if (!rhs.valid())
            return false;
        return file_ == rhs.file_ && line_ == rhs.line_ && ix_ == rhs.ix_;
    }
    void WidgetId::stream(std::ostream &os) const
    {
        if (!valid())
        {
            os << "Invalid WidgetId";
            return;
        }
        os << "WidgetId(" << file_ << ", " << line_ << ", " << ix_ << ")";
    }

} 
