// OPEN A NEW SKETCH WINDOW IN ARDUINO
// CLICK IN THIS BOX, CTL-A, CTL-C (Copy code)
// CLICK IN SKETCH, CTL-A, CTL-V (Paste code into sketch)

// file: Convolution.ino
// created by: Clark Hochgraf Sept 15, 2015
// modified by: David Orlicki Sept 1, 2017
// purpose: Convolution implementation
// x data stored in float, 
// h impulse response stored in float
// y result stored in float

unsigned long startTime, endTime, execTime; // micros() casting
const int IMP_RESP_LEN = 60;
const int DATA_LEN = 100;

// Initialize all arrays to zero
float xv[DATA_LEN]={0};    // convolution input:  x variable
float yv[DATA_LEN]={0};    // convolution output: y variable
float h[IMP_RESP_LEN]={0}; // filter impulse response 

void setup()
{
  // Begin serial output
  Serial.begin(9600);

  // Print table header
  Serial.println("x variable");
  Serial.println("n\txv[n]");

  // Initialize xv to a constant value of 5.0 with length DATA_LEN
  // and print to monitor. These values will come from the ADC later.
  for (int i = 0; i < DATA_LEN; i++)
  {
    xv[i] = 5.0;
    Serial.print(i); Serial.print('\t');
    Serial.println(xv[i]);
  }

  // Print table header
  Serial.println("\nh impulse response");
  Serial.println("n\th[n]");

  // Create the impulse response of the
  // filter via a moving average filter
  // and print values to terminal.
  for (int i = 0; i < IMP_RESP_LEN; i++)
  {
    h[i] = 1.0/IMP_RESP_LEN;  // Average
    Serial.print(i); Serial.print('\t');
    Serial.println(h[i],4);
  }

  // Log start time of convolution
  startTime = micros();

  // perform sum of products
  // start convolution only where data is valid
  // first IMP_RESP_LEN-1 datapoints are not valid
  for (int k = IMP_RESP_LEN-1; k < DATA_LEN; k++){
    for (int i = 0; i < IMP_RESP_LEN; i++){
      yv[k] = yv[k]+h[i]*xv[k-i];
    } 
  }

  // Log execution time for convolution
  execTime = micros()-startTime;
  
  Serial.println("\nN\txv\tyv");
  for (int i = 0; i < DATA_LEN; i++)
  {
    Serial.print(i); Serial.print('\t');        // print data number
    Serial.print(xv[i],4); Serial.print('\t');  // print convolution input
    Serial.println(yv[i],4);                    // print convolution output
  }

  Serial.println("\nFor given data type:");
  Serial.println("------------------------");
  Serial.print("Data length = ");
  Serial.println(DATA_LEN); 
  Serial.print("final value of yv = ");
  Serial.println(yv[DATA_LEN-1]);
  Serial.print("microseconds for each new datapoint = ");
  Serial.println(execTime/(DATA_LEN-(IMP_RESP_LEN-1)));
  Serial.print("Max update rate (Hz) = ");
  Serial.println(1000000/(execTime/(DATA_LEN-(IMP_RESP_LEN-1))));
}

void loop(){ } // spin forever
