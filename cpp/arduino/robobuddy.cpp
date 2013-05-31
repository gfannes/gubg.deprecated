#include "garf/Sonar.hpp"
#include "garf/Sonars.hpp"
#include "garf/Motor.hpp"
#include "garf/ObjectAvoidance.hpp"
#include "garf/IRSensor.hpp"
#include "Arduino.h"
#include "HMC5883L/HMC5883L.h"

using namespace garf;

// Reference the I2C Library

#include "Wire/Wire.h"
extern "C"
{
	#include "Wire/utility/twi.c"
}
// Reference the HMC5883L Compass Library
#include "HMC5883L/HMC5883L.h"
#include "HMC5883L/HMC5883L.cpp"
HMC5883L compass;


const int servo = 9;
const int totalDelay = 2000;
const int MaxSamples = 30;
int values[MaxSamples];
int nrSamples;

typedef Sonars<7, 2, 8, 200> S;
S sonars;
volatile unsigned long distance_l, distance_r;
S::Which wh;

garf::Motor<6, 5> g_rightMotor;
garf::Motor<10, 9> g_leftMotor;

class OA: public ObjectAvoidance_crtp<OA>
{
    public:
        OA():
            target_l(), target_r(), current_l(), current_r(), pm_(millis()){}

        void oa_left(int speed) { target_l = speed; }
        void oa_right(int speed) { target_r = speed; }
        unsigned long oa_leftDistance(){return distance_l;}
        unsigned long oa_rightDistance(){return distance_r;}
        unsigned long oa_thresholdDistance(){return 18;}

        void process()
        {
            ObjectAvoidance_crtp::process();

            auto cm = millis();
            if (cm-pm_ > 5)
            {
                pm_ = cm;
                if (current_l < target_l)
                    ++current_l;
                else if (current_l > target_l)
                    current_l -= 2;
                if (current_r < target_r)
                    ++current_r;
                else if (current_r > target_r)
                    current_r -= 2;

                g_leftMotor.setSpeed(current_l);
                g_rightMotor.setSpeed(current_r);
            }
        }

    private:
       int target_l; 
       int target_r; 
       int current_l; 
       int current_r; 
       unsigned int pm_;
};
OA oa;

class IR: public IRSensor<IR, 42>
{
    public:
        void irsensor_detect(bool b)
        {
            //Serial.println(b ? "Stoppen!" : "OK to go!");
        }
};
IR ir;

void setup()
{
    Serial.begin(9600);
    sonars = S();
    sonars.init();
    pinMode(13, OUTPUT);
    ir.init();
    oa.init();
    nrSamples = 0;

    /*
      // Initialize the serial port.

   Serial.println("Starting the I2C interface.");
   Wire.begin(); // Start the I2C interface.

   Serial.println("Constructing new HMC5883L");
   compass = HMC5883L(); // Construct a new HMC5883 compass.
    
   Serial.println("Setting scale to +/- 1.3 Ga");
   int error = compass.SetScale(1.3); // Set the scale of the compass.
   if(error != 0) // If there is an error, print it out.
     Serial.println(compass.GetErrorText(error));
  
   Serial.println("Setting measurement mode to continous.");
   error = compass.SetMeasurementMode(Measurement_Continuous); // Set the measurement mode to Continuous
   if(error != 0) // If there is an error, print it out.
     Serial.println(compass.GetErrorText(error));
     */
}
// Output the data down the serial port.
void Output(MagnetometerRaw raw, MagnetometerScaled scaled, float heading, float headingDegrees)
{
   Serial.print("Raw:\t");
   Serial.print(raw.XAxis);
   Serial.print("   ");   
   Serial.print(raw.YAxis);
   Serial.print("   ");   
   Serial.print(raw.ZAxis);
   Serial.print("   \tScaled:\t");
   
   Serial.print(scaled.XAxis);
   Serial.print("   ");   
   Serial.print(scaled.YAxis);
   Serial.print("   ");   
   Serial.print(scaled.ZAxis);

   Serial.print("   \tHeading:\t");
   Serial.print(heading);
   Serial.print(" Radians   \t");
   Serial.print(headingDegrees);
   Serial.println(" Degrees   \t");
}

void loop()
{
    /*
    ir.process();
  // Retrive the raw values from the compass (not scaled).
  MagnetometerRaw raw = compass.ReadRawAxis();
  // Retrived the scaled values from the compass (scaled to the configured scale).
  MagnetometerScaled scaled = compass.ReadScaledAxis();
  
  // Values are accessed like so:
  int MilliGauss_OnThe_XAxis = scaled.XAxis;// (or YAxis, or ZAxis)

  // Calculate heading when the magnetometer is level, then correct for signs of axis.
  float heading = atan2(scaled.YAxis, scaled.XAxis);
  
  // Once you have your heading, you must then add your 'Declination Angle', which is the 'Error' of the magnetic field in your location.
  // Find yours here: http://www.magnetic-declination.com/
  // Mine is: 2� 37' W, which is 2.617 Degrees, or (which we need) 0.0456752665 radians, I will use 0.0457
  // If you cannot find your Declination, comment out these two lines, your compass will be slightly off.
  float declinationAngle = 0.0457;
  heading += declinationAngle;
  
  // Correct for when signs are reversed.
  if(heading < 0)
    heading += 2*PI;
    
  // Check for wrap due to addition of declination.
  if(heading > 2*PI)
    heading -= 2*PI;
   
  // Convert radians to degrees for readability.
  float headingDegrees = heading * 180/M_PI; 

  // Output the data via the serial port.
//  Output(raw, scaled, heading, headingDegrees);

  // Normally we would delay the application by 66ms to allow the loop
  // to run at 15Hz (default bandwidth for the HMC5883L).
  // However since we have a long serial out (104ms at 9600) we will let
  // it run at its natural speed.
  // delay(66);
  */

#if 1
//    Serial.print((int)oa.debug_getState());
    unsigned long distance;
    if (sonars.process(wh, distance))
    {
        switch (wh)
        {
            case S::Which::Left:
                distance_l = distance;
/*
                Serial.print(distance);
                Serial.println("");
                */
                break;
            case S::Which::Right:
                distance_r = distance;
/*
                Serial.print("               ");
                Serial.print(distance);
                Serial.println("");
                */
                break;
        }
    }
    oa.process();

#endif
}
