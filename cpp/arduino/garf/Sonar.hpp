#ifndef HEADER_garf_Sonar_hpp_ALREADY_INCLUDED
#define HEADER_garf_Sonar_hpp_ALREADY_INCLUDED

namespace garf
{
    template <int Trigger, int Echo, int PulseStart, int PulseEnd>
        class Sonar
        {
            public:
                void init()
                {
                    pinMode(Trigger, OUTPUT);
                    pinMode(Echo, INPUT);
                }
                template <typename Distance>
                    int measure(Distance &distance)
                    {
                        digitalWrite(Trigger, LOW);
                        delay(10);
                        //pulse
                        digitalWrite(Trigger, HIGH);
                        delayMicroseconds(20);
                        digitalWrite(Trigger, LOW);
                        auto start=millis();

                        for (; digitalRead(Echo) == LOW;)
                        {
                            if (distance > PulseStart)
                                return 1;
                        }

                        for (; digitalRead(Echo) == HIGH; ++distance)
                        {
                            if (distance > PulseEnd)
                                return 2;
                        }

                        return 0;
                    }
        };
}

#endif
