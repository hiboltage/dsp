// OPEN NEW ARDUINO SKETCH.
// CLICK IN THIS TEXT BOX. CTRL-A, CTRL-C.
// CLICK IN SKETCH. CTRL-A, CTRL-V.

// file: Roundoff_1.ino
// created by: Clark Hochgraf  Sept 15, 2015
// modified by: David Orlicki  Sept 1, 2017
// purpose: Illustration of roundoff error when using float

const int NUM_CALC = 1000;
float xv = 0.0;
float A, B, error, scale_factor;

void setup()
{
  // Setup serial communications
  Serial.begin(9600);
  Serial.println(F("Lab 3 P2 datatypes 190928"));
  Serial.println(F("\nEnter 'g' to go ....."));
  
  // Wait until 'g' entry
  while (Serial.read() != 'g');

  // Seed random command used in random_float()
  randomSeed(425);

  scale_factor = 100000.0;
  xv = 1.0;

  // Print scale factor
  Serial.print("Adding random floating point numbers with scale factor ");
  Serial.println(scale_factor); 

  // Print initial value of xv
  Serial.print("x value starts at x = ");
  Serial.println(xv,10);  // print to 10 decimal places

  // Loop for NUM_CALC
  for (int i = 1; i < NUM_CALC; i++)
  {
    // Generate random floats A and B and scale them according to scale_factor
    A = random_float() * scale_factor;
    B = random_float() * scale_factor;

    // Determine roundoff error by adding A and B to a constant value and then subtracting them
    xv += A;   // add A to xv
    xv += B;   // add B to xv
    xv -= A;   // subtract A from xv
    xv -= B;   // subtract B from xv
  }

  // Compare the ideal hardcoded output value (1.0) and actual output value (xv) to determine the error
  error = 1.0 - xv;
  
  // end result should just be the original value of x (i.e. 1.0)
  Serial.print("x value finishes at x = "); Serial.println(xv,10);

  // Print percent error
  Serial.print("Total percent error after 1000 calculations = ");
  Serial.print(error*100.0,10); Serial.println(" %");

  // Print average percent error
  Serial.print("Average percent error per addition or subtraction = ");
  Serial.print(error*100.0/(4.0*NUM_CALC),10); Serial.println(" %");
  Serial.println(); 
} // setup()

void loop(){} // spin forever

//*********************************************************************
float random_float() { return (random(2147483648)/2147483648.0); }
