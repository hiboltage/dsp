// OPEN A NEW SKETCH WINDOW IN ARDUINO
// CLICK IN THIS BOX, CTL-A, CTL-C (Copy code from text box.)
// CLICK IN SKETCH, CTL-A, CTL-V (Paste code into sketch.)

// file: Lab4_FIR_Section1_BreathingModel.ino

// created by: Clark Hochgraf and David Orlicki Oct 18, 2018
// Modified by: Mark Thompson 2/15/2020
//              Added multiple comments

#include <MsTimer2.h>


//  Define some constants as integers
//
//  Some Arduino values
//  V_REF -- Reference voltage for the ADC
//  DAC0 -- IO Pin for the hardware DAC input 0
//  DAC1 -- IO Pin for the hardware DAC input 1
//  DAC2 -- IO Pin for the hardware DAC input 2
//
//  TSAMP_MSEC -- Sample time in milliseconds
//  DATA_LEN -- number of samples to compute
//  DATA_EXPT -- Value to multiply the floating point number by 
//            when representing as a fixed pointexponent.

const int V_REF = 5.0, DAC0 = 3, DAC1 = 4, DAC2 = 5;
const int TSAMP_MSEC = 10, DATA_LEN = 256;
const long DATA_FXPT = 1;

// Define variables
// sampleFlag -- Boolean set when it is time to sample
// fltx -- Floating point X value
// tick -- Sample number
// i  -- General index
// execUsec -- execution time in microseconds


volatile boolean sampleFlag = false;
float fltX;
int tick = 0, i, fxptX;
unsigned long execUsec;

//*******************************************************************
void setup()
{
  
  
//  Set up the serial port and display a header

  Serial.begin(115200); delay(20);


  
//  Declare hardware IO for the Arduino
  
  pinMode(DAC0,OUTPUT); digitalWrite(DAC0,LOW);
  pinMode(DAC1,OUTPUT); digitalWrite(DAC1,LOW);
  pinMode(DAC2,OUTPUT); digitalWrite(DAC2,LOW);

  // Use default 5V ADC reference (beware Vcc supply noise)
  analogReference(DEFAULT); // 5.0 volt ADC reference
  analogRead(0); // prime ADC registers with discarded read

// Handshake with MATLAB      
  Serial.println(F("%Arduino Ready"));
  while (Serial.read() != 'g'); // spin
  
   //  Set up the interrupt timer

  MsTimer2::set(TSAMP_MSEC, ISR_Sample); // Set sample msec, ISR name
  MsTimer2::start();
}

//*******************************************************************
void loop()
{
  syncSample(); 
  
  //  Function call to read from the ADC performing dither and oversample averaging
  //fltX = analogReadDitherAveDegC();
  
  //  Function call to compute a value from the breathing model rather than the actual ADC output
  fltX = simulatedBreathingDegC();
  
  //  Convert the floating point number to a fixed point value. First
  //  scale the floating point value by a number to increase its resolution
  //  (use DATA_FXPT).  Then round the value and truncate to a fixed point
  //  INT datatype
  
  fxptX = int(DATA_FXPT*fltX + 0.5);
  
  displayData(fltX, fxptX);
  
  //  Test for having generated enough data, print the execution time, then just spin
  if (++tick >= DATA_LEN) while (true); // freeze process (spin forever)

} // loop()

//**************************************************************
float analogReadDitherAveDegC(void)
{ 

//  Function that performs dithering and oversample averaging on values
//  read from the ADC.  This is done in a loop and setting the output
//  of the DAC values to the loop iterator (i).  Then read from the ADC
//  and accumulate the ADC samples.  Finally divide by the number of samples
//  taken to compute the average.

  const int NUM_SUBSAMPLES = 8;
  float sum = 0.0, adcCV, snsVolts, degC;
  
  for (i = 0; i < NUM_SUBSAMPLES; i++) {
    //  Write to the DAC outputs one bit at a time using a mask on the iterator
    //  i to set each individual bit
    int ramp = i % 8; // set to 0 for no dither, average
    digitalWrite(DAC0, (ramp & B00000001)); // LSB bit mask
    digitalWrite(DAC1, (ramp & B00000010));
    digitalWrite(DAC2, (ramp & B00000100)); // MSB bit mask
  
    //  Read from the ADC and accumulate the sum of all the values
    sum += analogRead(A0);
  }
  
  //  Divide by N the number of samples in the OSA
  adcCV = sum/NUM_SUBSAMPLES;
  
  //  Convert the ADC code values to voltage
  snsVolts = (adcCV/1024)*V_REF;
  
  //  Convert the voltage to temperature
  degC = 100.0*(snsVolts - 0.6); // 10 mV/degC -> 100 degC/V
  
  return degC;   
}

//*******************************************************************
float simulatedBreathingDegC(void) // handout starter code
{ 
  float degC = 0.0;

  float driftIntercept;
  float driftSlope;
  float samplesPerCycle;
  float tempP;
  float bpm;
  float sigma;
  
  //  Create a simulated breath output value using the equation
  //
  //  n is the sample number 
  //
  //  driftSlope is in degrees per sample
  //
  //  tempP is the peak temperature variation
  //
  //  samplesPerCycle is the period of the sinusoid of the breathing rate
  //  This translates to a breathing rate in bpm of
  //     bpm = 1/samplesPerCycle * 10Sec/Sample * 60 Sec/Min
  //
  //  randNoise -- a uniformly distributed number from +/- (.5/.29 ) so it has
  //      a standard deviation of 1 degree C
  //
  //  degC = driftIntercept + ( driftSlope * n ) + tempP * sin( 2pi * n / ticksPerCycle )
  //         + sigma * randNoise

  //  Replace these values with values that match your model
  driftIntercept = 31.8653;
  driftSlope = 0.0243;
  bpm = 15.3846;
  tempP = 1.3522;
  sigma = 0.1;

  //  Convert BPM to samples per cycle
  samplesPerCycle = 10 * 60 / bpm;
  
  degC = driftIntercept; // driftIntercept value
  degC += driftSlope*(tick); // driftSlope in degC/sample
  degC += tempP*sin(TWO_PI*tick/samplesPerCycle);
  degC += sigma*((random(0,11)/10.0-0.5)/0.29); // scaled 1 degC  * sigma
  return degC;
}

//*******************************************************************
void displayData(float fltX, int fxptX)
{
  float scaleFltX = DATA_FXPT*fltX;
  float err = scaleFltX-fxptX;
  
  if (tick == 0) Serial.print(F("\nn\tfltX\n"));
  Serial.print(tick);        Serial.print('\t');
  Serial.print(fltX,3);  Serial.print('\n'); // base sample
}
//*******************************************************************
void syncSample(void)
{
  while (sampleFlag == false); // spin until ISR trigger
  sampleFlag = false;          // disarm flag: enforce dwell
}
//*******************************************************************
void ISR_Sample() { sampleFlag = true; }
