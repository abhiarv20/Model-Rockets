//load cell defined
#define DT 4  //A4 A0 4
#define SCK 5 //A5 A1 5


long sample=0;
float val=0;
long count=0;
 
unsigned long readCount(void)
{
unsigned long Count;
unsigned char i;
pinMode(DT, OUTPUT);
digitalWrite(DT,HIGH);
digitalWrite(SCK,LOW);
Count=0;
pinMode(DT, INPUT);
while(digitalRead(DT));
for (i=0;i<24;i++)
{
digitalWrite(SCK,HIGH);
Count=Count<<1;
digitalWrite(SCK,LOW);
if(digitalRead(DT))
Count++;
}
digitalWrite(SCK,HIGH);
Count=Count^0x800000;
digitalWrite(SCK,LOW);
return(Count);
}


//temp sensor defined
const int lm35_1 = A0;
const int lm35_2 = A1;
const int lm35_3 = A2;
//const int lm35_4 = A3;
int temp_adc_val;
float temp_val;

//set led/ignitor state
int Ignitor_state = LOW;

//set led indicator state
int Led_state = LOW;
unsigned long prev_time = millis()/1000;


void setup() {
  // put your setup code here, to run once:

  //intialise load cell
  pinMode(SCK, OUTPUT);
  Serial.begin(9600);
  Serial.print("Weight Measurement \t");
  delay(1000);
  calibrate();

  //insitialise temperature sensor
  pinMode(lm35_1,INPUT);
  pinMode(lm35_2,INPUT);
  pinMode(lm35_3,INPUT);
  //pinMode(lm35_4,INPUT);

  //insitialise led/ignitor
  pinMode(13,OUTPUT); 

  //insitialise led indicator
  pinMode(12,OUTPUT);
  
  Serial.begin(9600); // initialize serial communication:

}

void loop() {
  // put your main code here, to run repeatedly:

  Serial.println();
  //code for timer
  long int current_time = millis()/1000;//time in seconds
  Serial.print("current_time=\t");
  Serial.print(current_time);
  Serial.println();

  
  
  //code for load cell
  count= readCount();
  int w=(((count-sample)/val)-2*((count-sample)/val));
  Serial.print("Measured Weight \t");
  Serial.print(w);
  Serial.print("g");
  Serial.println();


  
  Serial.print('\t');
  
  

  //temp sensor 1 reading values
  temp_adc_val = analogRead(lm35_1);  /* Read Temperature temp_adc_val=analog to digital converted value  */                
  //Serial.print("ADC = ");
  //Serial.println(temp_adc_val);
  temp_val = (temp_adc_val * 4.88); /* Convert adc value to equivalent voltage  0.00488 volts to 4.88 mV */
  temp_val = (temp_val/10); /* LM35 gives output of 10mv/°C */
  Serial.print("Temp_01 = \t");
  Serial.print(temp_val);
  Serial.print("  ");
  //Serial.print(" Degree Celsius");

  //temp sensor 2 reading the values
  temp_adc_val = analogRead(lm35_2);
  temp_val = (temp_adc_val * 4.88);
  temp_val = (temp_val/10);
  Serial.print("Temp_02 = \t");
  Serial.print(temp_val);
  Serial.print("  ");

  temp_adc_val = analogRead(lm35_3);
  temp_val = (temp_adc_val * 4.88);
  temp_val = (temp_val/10);
  Serial.print("Temp_03 = \t");
  Serial.print(temp_val);
  Serial.print("  ");

  /*
  temp_adc_val = analogRead(lm35_4);
  temp_val = (temp_adc_val * 4.88);
  temp_val = (temp_val/10);
  Serial.print("Temp_04 = \t");
  Serial.print(temp_val);
  */
  
  //code for ignitor triggering
  if(current_time == 40){
    Ignitor_state = !Ignitor_state;
    digitalWrite(13,Ignitor_state);
    Serial.print("ignitor triggerd \t");
  }
  //code for ignitor triggering switch off
  if(current_time == 42){
    Ignitor_state = !Ignitor_state;
    digitalWrite(13,Ignitor_state);
    Serial.print("ignitor triggerd off \t");
  }

  //code for led indicator
  if(current_time - prev_time > 1){
    Led_state = !Led_state;
    digitalWrite(12,Led_state);
    prev_time = current_time;
  }
  
  Serial.println();
  //delay(200);
}



void calibrate(){


  Serial.print("calibrating..Please Wait..\t");
  Serial.println();
  for(int i=0;i<100;i++)
  {
  count=readCount();
  sample+=count;
  }
  sample/=100;

  Serial.print("Put 100g weight");
  Serial.println();
  count=0;
  while(count<1000)
  {
  count=readCount();
  count=sample-count;
  }

  Serial.print("Please Wait...");
  //Serial.println();

  delay(1000);
  for(int i=0;i<100;i++)
  {
  count=readCount();
  val+=sample-count;
  }
  val=val/100.0;
  val=val/100.0; // put here your calibrating weight
}
