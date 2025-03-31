// OPEN NEW ARDUINO SKETCH.
// CLICK IN THIS TEXT BOX. CTRL-A, CTRL-C.
// CLICK IN SKETCH. CTRL-A, CTRL-V.

// Lab2_handout.ino
// created by: Clark Hochgraf 2015
// modified by: David Orlicki, October 1, 2017
// modified by: David Orlicki, September 7, 2019
// modified by: Mark Thompson, September 8, 2020

// Outputs 3 bit DAC resistor network drive.
// Reads/displays ADC samples of LM61 sensor output.
// Reads/displays averaged subsamples of LM61 sensor output.
// Reads/displays dithered-averaged subsamples of LM61 sensor output.
// Calculates running mean and standard deviation of samples.

#include <MsTimer2.h>
#include <SPI.h>

const int TSAMP_MSEC = 100; // sensor: 100, sim: 10
const int DAC0 = 3, DAC1 = 4, DAC2 = 5, LM61 = A0, VDITH = A1;
const int NUM_SUBSAMPLES = 160, NUM_SAMPLES = 256;

// Values taken from description (page 1) of LM61 datasheet
// https://www.ti.com/lit/ds/symlink/lm61.pdf?ts=1728062559449&ref_url=https%253A%252F%252Fwww.google.com%252F
const float DC_OFFSET = 0.600;
const float VOLTAGE_PROPORTION = 0.010;

// Value taken from "Analog-to-Digital Converter" section (Page 205) of https://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-7810-Automotive-Microcontrollers-ATmega328P_Datasheet.pdf
// Resolution is 10 bits, 2^10 = 1024
const int ADC_RES = 1024;

// Value guesstimated from "ADC Voltage Reference section" (Page 211) of https://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-7810-Automotive-Microcontrollers-ATmega328P_Datasheet.pdf
// and Avcc pin on Arduino Uno Rev3 schematic (https://www.arduino.cc/en/uploads/Main/Arduino_Uno_Rev3-schematic.pdf)
const float VREF = 5.00;

volatile boolean sampleFlag = false;
int tick = 0;
float sample[3] = {};  // initialize all values of sample array to zero
float mean, stdev;

//*******************************************************************
void setup()
{
  configureArduino();  // sets DAC0, DAC1, and DAC2 as outputs and writes them low

  Serial.println("%Arduino Ready");  // This line tells MATLAB that the Arduino is ready to accept data
  
  //Serial.println("Enter 'g' to go .....");

  // Wait until MATLAB sends a 'g' to start sending data
  while (Serial.read() != 'g'); // spin until 'g' entry  
  
  MsTimer2::set(TSAMP_MSEC, ISR_Sample); // Set sample rate in msec and ISR function name
  MsTimer2::start(); // start running the Timer   
} // setup()

//*******************************************************************
// Main program
void loop()
{
  //float* sample;  // a pointer to a float
  // Synchronizes main program refresh rate with MsTimer2
  syncSample();  // wait until MsTimer2 calls ISR_Sample() which sets sampleFlag true
  
  //sample = analogRead(LM61);
  //sample = analogReadAve();
  //sample = testDAC(random(NUM_SAMPLES));
  analogReadDitherAve(sample);
  //sample = 185-0*tick/32.0+2*sin((tick/32.0)*TWO_PI);

  // recurseStats1(sample, mean, stdev);
  // recurseStats2(sample, mean, stdev);
  
  displayData();  // print sampled data to serial port for MATLAB
  if (++tick >= NUM_SAMPLES) while(true); // if number of samples has been completed, wait forever
} // loop()

//*******************************************************************
float analogReadAve(void)
{
  float sum = 0.0;
  for (int i = 0; i < NUM_SUBSAMPLES; i++) sum += analogRead(LM61);
  return sum/NUM_SUBSAMPLES; // averaged subsamples   
}

//*******************************************************************
float testDAC(int index)
{
  // Write to the test DAC digital I/O pins to create dither noise
  digitalWrite(DAC0, (index & B00000001));  // write to pin 3 - LSB bit mask
  digitalWrite(DAC1, (index & B00000010));  // write to pin 4
  digitalWrite(DAC2, (index & B00000100));  // write to pin 5 - MSB bit mask
  return analogRead(VDITH);                 // read analog value from pin A1
}

//*******************************************************************
// Function that adds dither noise and OSA to three different ADC output types
// Output parameter array is used to pass the output array by reference to the sample array
void analogReadDitherAve(float output[3])
{
  float sampleRead = 0.0;   // initialize variable to read a single ADC value
  float sampleVolts = 0.0;  // initialize variable to convert a single ADC value to mV
  float sumADC = 0.0;    // initialize sum for output in adc values
  float sumVolts = 0.0;  // initialize sum for output in mV
  float sumTemp = 0.0;   // initialize sum for output in degC
  float sum = 0.0;
  
  // Loop for number of subsamples to collect
  // This is oversampling where NUM_SUBSAMPLES are collected and averaged for every sample in NUM_SAMPLES
  for (int i = 0; i < NUM_SUBSAMPLES; i++){
    
    // Create dither noise by writing random values to the DAC digital I/O pins
    digitalWrite(DAC0, (i & B00000001));  // write to pin 3 - LSB bit mask
    digitalWrite(DAC1, (i & B00000010));  // write to pin 4
    digitalWrite(DAC2, (i & B00000100));  // write to pin 5 - MSB bit mask
    
    sampleRead = analogRead(LM61);  // read single subsample from LM61 (pin A0)
    sampleVolts = (sampleRead * VREF) / ADC_RES;  // convert subsample to volts
    sumADC += sampleRead;  // add subsample to adc values output sum
    sumVolts += (sampleVolts * 1000.00); // convert voltage subsample to mV and add to mV output sum
    sumTemp += ((sampleVolts - DC_OFFSET) / VOLTAGE_PROPORTION);  // convert subsample to degC and add to degC output sum
  }
  
  // Average sums over number of subsamples collected and pack into output array
  output[0] = sumADC / NUM_SUBSAMPLES;
  output[1] = sumVolts / NUM_SUBSAMPLES;
  output[2] = sumTemp / NUM_SUBSAMPLES;
}

//*******************************************************************
void recurseStats1(float smpl, float &mean, float &stdev)
{
  // Smith algorithm adjusted for tick start at 0
  static float sum, sumSquares;
  
  if (tick == 0)
  {
    mean = smpl;
    sum = smpl;
    sumSquares = smpl*smpl;
  }
  else 
  {
   sum += smpl;
   sumSquares += smpl*smpl;
   mean = sum/(tick+1);
   float var = (sumSquares - (sum*sum)/(tick+1))/tick;
   stdev = sqrt(var);    
  }  
}

//*******************************************************************
void recurseStats2(float smpl, float &mean, float &stdev)
{
  // B. P. Welford algorithm adjusted for tick start at 0
  static float oldMean, runSumVar, oldRunSumVar;
  
  mean = 0.0;
  stdev = 0.0;
}

//*******************************************************************
// Function to print values into MATLAB
void displayData(void)
{
  // Poor man's data table
  if (tick == 0) Serial.print("\nn\tADC\tmV\tdegC\n");   // if first sample, print data header
  Serial.print(tick);        Serial.print('\t');                      // print current tick and then tab
  Serial.print(sample[0]);   Serial.print('\t'); Serial.print('\t');  // print sample in adc values and then two tabs
  Serial.print(sample[1]);   Serial.print('\t');  // print sample in mV and then tab
  Serial.print(sample[2]);   Serial.print('\n');  // print sample in degC and then newline
}

//*******************************************************************
void syncSample(void)
{
  while (sampleFlag == false); // spin until ISR trigger
  sampleFlag = false;          // disarm flag: enforce dwell  
}

//*******************************************************************
void configureArduino(void)
{
  pinMode(DAC0,OUTPUT); digitalWrite(DAC0,LOW);
  pinMode(DAC1,OUTPUT); digitalWrite(DAC1,LOW);
  pinMode(DAC2,OUTPUT); digitalWrite(DAC2,LOW);
  analogReference(DEFAULT); // DEFAULT, INTERNAL
  analogRead(LM61); // read and discard to prime ADC registers
  Serial.begin(115200); // 11 char/msec 
}

//*******************************************************************
void ISR_Sample()
{
  sampleFlag = true;
}
