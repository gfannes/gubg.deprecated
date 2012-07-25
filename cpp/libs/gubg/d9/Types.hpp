#ifndef gubg_d9_Types_hpp
#define gubg_d9_Types_hpp

namespace gubg
{
    namespace d9
    {
        typedef unsigned char ubyte;

        struct Byte
        {
            const static ubyte D8 =   0xd8;
            const static ubyte D9 =   0xd9;
            const static ubyte End =  0xff;
            const static ubyte Ack =  0xfe;
            const static ubyte Nack = 0xfd;
        };
    }
}

#endif
