#include "gubg/Testing.hpp"
#include "gubg/Settings.hpp"

enum Key {Name, Includes, Options};

typedef gubg::Settings<Key> Settings;

int main()
{
    TEST_TAG(Settings);
    Settings settings;
    TEST_TRUE(settings.set(Name, "Root"));
    TEST_FALSE(settings.set(Name, "Root"));
    return 0;
}
