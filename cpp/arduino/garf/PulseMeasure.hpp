#ifndef HEADER_garf_PulseMeasure_hpp_ALREADY_INCLUDED
#define HEADER_garf_PulseMeasure_hpp_ALREADY_INCLUDED

#include "Arduino.h"

namespace pulseMeasure
{
	const int inputCapturePin = 8;
	const int prescale = 8;
	const byte prescaleBits = B010;
	const long precisision = (1000000/(F_CPU/1000)) * prescale;
		
	volatile unsigned int start = 0;
	volatile unsigned int end = 0;

	void init()
	{
		start = 0;
		end = 0;
					
		TCCR1A = 0;				//Normal counting mode
	 	TCCR1B = prescaleBits;	// 
		TCCR1B |= _BV(ICES1);	// enable input capture
	
		bitSet(TIMSK1 , ICIE1);	// enable input capture interrupt for timer1
		TCNT1 = 0;				// reset the counter
	}
	
	void isr()
	{
		if(bitRead(TCCR1B, ICES1))
		{
			start = ICR1;			// get the capture value
			TCCR1B ^= _BV(ICES1);	// toggle bit to capture falling edge
		}
		else
		{
			end = ICR1;
			bitClear(TIMSK1, ICIE1);
		}
	}
}

ISR(TIMER1_CAPT_vect)
{
	pulseMeasure::isr();	
}


#endif
