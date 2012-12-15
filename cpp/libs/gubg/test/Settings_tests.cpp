#include "gubg/Testing.hpp"
#include "gubg/Settings.hpp"
using namespace std;

enum Key {Name, Includes, Options};

typedef gubg::Settings<Key> Settings;

int main()
{
    TEST_TAG(Settings);
    Settings settings;
    TEST_TRUE(settings.set(Name, "Root"));
    TEST_FALSE(settings.set(Name, "Root"));
    string str;
    TEST_TRUE(settings.get(str, Name));
    TEST_EQ("Root", str);
    TEST_TRUE(settings.add(Includes, "aaa"));
    TEST_TRUE(settings.add(Includes, "bbb"));
    TEST_TRUE(settings.add(Includes, "ccc"));
    vector<string> strs;
    TEST_TRUE(settings.get(strs, Includes));
    TEST_TRUE((strs == Settings::Strings({"aaa", "bbb", "ccc"})));
    return 0;
}
