#include <Servo.h>

//for servo motor
Servo myservo;  // create servo object to control a servo
// twelve servo objects can be created on most boards
int pos = 0;    // variable to store the servo position


//for pressure and altitude
//Any Arduino pins labeled:  SDA  SCL
//Uno, Redboard, Pro:        A4   A5
// Your sketch must #include this library, and the Wire library.
// (Wire is a standard library included with Arduino.):
#include <SFE_BMP180.h>
#include <Wire.h>
// You will need to create an SFE_BMP180 object, here called "pressure":
SFE_BMP180 pressure;
double baseline; // baseline pressure


//for accelerometer
float x,y,z;


//set led indicator state
int Led_state = LOW;
unsigned long prev_time = millis()/1000;

//for servorotate function
boolean alreadyRun = false;

void setup() {

  //pin for servo motor
  myservo.attach(9);  // attaches the servo on pin 9 to the servo object


  //begin altitude pressure and temperature sensor
  Serial.begin(9600);
  Serial.println("REBOOT");
  // Initialize the sensor (it is important to get calibration values stored on the device).
  if (pressure.begin())
    Serial.println("BMP180 init success");
    //can write code for led blink to indicate
  else
  {
    // Oops, something went wrong, this is usually a connection problem,
    // see the comments at the top of this sketch for the proper connections.

    Serial.println("BMP180 init fail (disconnected?)\n\n");
    while(1); // Pause forever.
  }
  // Get the baseline pressure:  
  baseline = getPressure();
  Serial.print("baseline pressure: ");
  Serial.print(baseline);
  Serial.println(" mb");  

  //insitialise led indicator
  pinMode(10,OUTPUT);

}

void loop() {

  Serial.println();
  //code for timer
  long int current_time = millis()/1000;//time in seconds
  Serial.print("current_time=\t");
  Serial.print(current_time);
  Serial.println();
  

  
  //for pressure and altitude loop
  double a,P,current_altitude,prev_altitude;

  // Get a new pressure reading:
  P = getPressure();
  // Show the relative altitude difference between
  // the new reading and the baseline reading:
  a = pressure.altitude(P,baseline);  
  current_altitude = a;
  Serial.print("relative altitude: ");
  if (a >= 0.0) Serial.print(" "); // add a space for positive numbers
  Serial.print(a,1);
  Serial.print(" meters, ");
  if (a >= 0.0) Serial.print(" "); // add a space for positive numbers
  Serial.print(a*3.28084,0);
  Serial.println(" feet");

  

  //for accelerometer
  x = ((analogRead(A0)*2.0)/1024.0)-1;  
  y = ((analogRead(A1)*2.0)/1024.0)-1;
  z = ((analogRead(A2)*2.0)/1024.0)-1; 

  Serial.print("gX = ");
  Serial.print(x);
  Serial.print(" ");
  Serial.print("gY = ");
  Serial.print(y);
  Serial.print(" ");
  Serial.print("Z = ");
  Serial.print(z);
  Serial.print(" "); 
  


  //1st condition for servo to rotate
  //rotate only once that's the problem
  
  if(current_altitude - prev_altitude < 0){
    if (alreadyRun == false)
    {
      servorotate();//calling servo rotate function
    }
    prev_altitude = current_altitude;
  } 
   prev_altitude = current_altitude;
  
  //2nd condition for servo ejection
  if(current_time == 10){
    Serial.print("rotate servo");
    for (pos = 0; pos <= 90; pos += 1) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15ms for the servo to reach the position
    }
  }

  //3rd condition for servo ejection
  //problem is it rotates always but it should rotate once
  if(z > 0){
    if (alreadyRun == false)
    {
      servorotate();//calling servo rotate function
    }
    
  }


  //code for led indicator
  if(current_time - prev_time > 1){
    Led_state = !Led_state;
    digitalWrite(10,Led_state);
    prev_time = current_time;
  }

  Serial.println();
  delay(1000);
  
}


//servo rotate function
void servorotate(){
  Serial.print("rotate servo");
  for (pos = 0; pos <= 90; pos += 1) { // goes from 0 degrees to 180 degrees
  // in steps of 1 degree
  myservo.write(pos);              // tell servo to go to position in variable 'pos'
  delay(15);                       // waits 15ms for the servo to reach the position
 
  alreadyRun = true;//setting flag value true
  }
}

  
//to get pressure from sensor
double getPressure()
{
  char status;
  double T,P,p0,a;

  // You must first get a temperature measurement to perform a pressure reading.
  
  // Start a temperature measurement:
  // If request is successful, the number of ms to wait is returned.
  // If request is unsuccessful, 0 is returned.

  status = pressure.startTemperature();
  if (status != 0)
  {
    // Wait for the measurement to complete:

    delay(status);

    // Retrieve the completed temperature measurement:
    // Note that the measurement is stored in the variable T.
    // Use '&T' to provide the address of T to the function.
    // Function returns 1 if successful, 0 if failure.

    status = pressure.getTemperature(T);
    if (status != 0)
    {
      // Start a pressure measurement:
      // The parameter is the oversampling setting, from 0 to 3 (highest res, longest wait).
      // If request is successful, the number of ms to wait is returned.
      // If request is unsuccessful, 0 is returned.
      Serial.print("temperature: ");
      Serial.print(T,2);
      Serial.println(" deg C, ");

      status = pressure.startPressure(3);
      if (status != 0)
      {
        // Wait for the measurement to complete:
        delay(status);

        // Retrieve the completed pressure measurement:
        // Note that the measurement is stored in the variable P.
        // Use '&P' to provide the address of P.
        // Note also that the function requires the previous temperature measurement (T).
        // (If temperature is stable, you can do one temperature measurement for a number of pressure measurements.)
        // Function returns 1 if successful, 0 if failure.

        status = pressure.getPressure(P,T);
        if (status != 0)
        {
          return(P);
        }
        else Serial.println("error retrieving pressure measurement\n");
      }
      else Serial.println("error starting pressure measurement\n");
    }
    else Serial.println("error retrieving temperature measurement\n");
  }
  else Serial.println("error starting temperature measurement\n");
}
