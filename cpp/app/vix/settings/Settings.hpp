#ifndef vix_settings_Settings_hpp
#define vix_settings_Settings_hpp

#include "Singleton.hpp"
#include <string>

namespace vix
{
    class Settings
    {
        private:
            friend class gubg::Singleton<Settings>;
            Settings(){}

        public:
            static Settings &instance(){return gubg::Singleton<Settings>::instance();}

            bool view(const std::string &filepath) const;
            bool edit(const std::string &filepath) const;
            bool open(const std::string &filepath) const;
    };
}

#endif
