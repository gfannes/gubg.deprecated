#ifndef gubg_Settings_hpp
#define gubg_Settings_hpp

#include <memory>
#include <vector>
#include <string>
#include <map>

namespace gubg
{
    template <typename Key>
        class Settings
        {
            public:
                Settings():infoPerKey_(new InfoPerKey){}

                bool set(Key key, const std::string &str)
                {
                    auto it = infoPerKey_->find(key);
                    if (it != infoPerKey_->end())
                        //Key already exists
                        return false;
                    (*infoPerKey_)[key] = createInfo_(str);
                    return true;
                }

            private:
                enum Type {Internal, String, Strings};
                struct Info
                {
                    Info(const std::string &str):type_(String), string_(str){}
                    Type type_;
                    Settings<Key> settings_;
                    std::string string_;
                    std::vector<std::string> strings_;
                };
                static std::shared_ptr<Info> createInfo_(const std::string &str){return std::shared_ptr<Info>(new Info(str));}
                typedef std::map<Key, std::shared_ptr<Info>> InfoPerKey;
                std::shared_ptr<InfoPerKey> infoPerKey_;
        };
}

#endif
