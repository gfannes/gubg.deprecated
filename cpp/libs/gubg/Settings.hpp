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
                typedef std::string String;
                typedef std::vector<String> Strings;

                Settings():infoPerKey_(new InfoPerKey){}

                bool set(Key key, const String &str)
                {
                    auto it = infoPerKey_->find(key);
                    if (it != infoPerKey_->end())
                        //Key already exists
                        return false;
                    (*infoPerKey_)[key] = createString_(str);
                    return true;
                }
                bool add(Key key, const String &str)
                {
                    auto it = infoPerKey_->find(key);
                    if (it == infoPerKey_->end())
                    {
                        (*infoPerKey_)[key] = createStrings_();
                        it = infoPerKey_->find(key);
                    }
                    Info &info = *it->second;
                    if (!info.is(Strings_))
                        return false;
                    info.strings_.push_back(str);
                    return true;
                }
                bool get(String &str, Key key) const
                {
                    auto it = infoPerKey_->find(key);
                    if (it == infoPerKey_->end())
                        //Key does not exist
                        return false;
                    const Info &info = *it->second;
                    if (!info.is(String_))
                        return false;
                    str = info.string_;
                    return true;
                }
                bool get(Strings &strs, Key key) const
                {
                    auto it = infoPerKey_->find(key);
                    if (it == infoPerKey_->end())
                        //Key does not exist
                        return false;
                    const Info &info = *it->second;
                    if (!info.is(Strings_))
                        return false;
                    strs = info.strings_;
                    return true;
                }

            private:
                enum Type {Internal, String_, Strings_};
                struct Info
                {
                    Info(const String &str):type_(String_), string_(str){}
                    Info():type_(Strings_){}
                    bool is(Type type) const {return type_ == type;}
                    Type type_;
                    Settings<Key> settings_;
                    String string_;
                    Strings strings_;
                };
                static std::shared_ptr<Info> createString_(const String &str){return std::shared_ptr<Info>(new Info(str));}
                static std::shared_ptr<Info> createStrings_(){return std::shared_ptr<Info>(new Info());}
                typedef std::map<Key, std::shared_ptr<Info>> InfoPerKey;
                std::shared_ptr<InfoPerKey> infoPerKey_;
        };
}

#endif
