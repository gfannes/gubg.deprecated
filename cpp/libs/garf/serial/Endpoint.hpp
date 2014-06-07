#include "Arduino.h"
#include "gubg/OnlyOnce.hpp"

namespace garf { namespace serial { 

    template <typename Receiver>
        class Endpoint_crtp
        {
            public:
                void process()
                {
                    if (doSetup_())
                        //Lazy setup, making sure the Arduino lib itself is already fully setup
                        Serial.begin(9600);

                    if (!Serial.available())
                        //Nothing to read
                        return;

                    receiver_().serialendpoint_process(Serial.read());
                }

                //Forwards everything to Serial.print(), variadic-style
                void print() const {}
                template <typename T, typename ...Rest>
                    void print(T t, Rest... rest) const { Serial.print(t); print(rest...); }

                void newline() const {print("\r\n");}

            private:
                Receiver &receiver_() {return static_cast<Receiver&>(*this);}

                gubg::OnlyOnce doSetup_;
        };

} } 
