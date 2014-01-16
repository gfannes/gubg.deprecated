#ifndef aoeuaou
#define aoeuaou

//In the Message logging framework, a static level is attached to a Message instance,
//the Module type, and the Stream type. If the combined message and moduled level is
//at least as high as the stream level, actual logging will happen.
//
//To make sure the level values for the Module and Stream types won't clash due to inheritance,
//type traits are used to specify level to these types; default is 0.

namespace gubg { namespace log {

    //Helper traits template that attaches an int value of 0 to every type by default
    template <typename T>
        struct Level
        {
            static int value;
        };
    template <typename T>
        int Level<T>::value = 0;

    //Provides a stream with the default level mechanics: the combined log and module level should at least equal the stream level
    template <typename Stream>
        struct LeveledStream: Stream
    {
        template <typename Module>
            static bool doLog(int level)
            {
                return level + Level<Module>::value >= Level<Stream>::value;
            }
    };


    //Some basic doLog() implementations
    struct Always
    {
        template <typename Module>
            static bool doLog(int) {return true;}
    };
    struct Never
    {
        template <typename Module>
            static bool doLog(int) {return false;}
    };
    template <int Lvl>
        struct IfAboveLevel
        {
            template <typename Module>
                static bool doLog(int lvl) {return lvl >= Lvl;}
        };

} }

#endif
