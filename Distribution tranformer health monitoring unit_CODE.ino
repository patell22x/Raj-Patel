/*******************Including Required Libraries********/
#include<EmonLib.h>   // For calculating the current and voltage value..as well as the powerfactor
#include<LiquidCrystal.h>  // For LCD monitor display
#include<DataCoder.h>     // For the RF transmission
#include<VirtualWire.h>   // For the RF transmission
#include<OneWire.h>       // For the proper definition of the sensor pin. useful in Dallas library
#include<DallasTemperature.h>   // Required library for the DS18B20 temperature sensor

/*********************************************************/

/**********************Defining Objects******************/
EnergyMonitor emon;
LiquidCrystal d(12,11,5,4,3,2);
#define ONE_WIRE_BUS 26
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
/*******************************************************/


/***********************Global Variables**************/
double S=0;   // Apparent Power
double P=0;   // Real Power
double Q=0;   // Reactive Power
double voltage; // For taking in voltage value
double current; // For taking in current value
double pF;    // Power Factor
double Time;     // Time for breaking 
double start;    // Reference time for breaking
double temperature;   // Representing the oil temperature
float level;     // Representing oil level
int stat=0;      // Keeping tab if the breaking count has started or not
int breakerOne = 50;    //  BREAKER No. 1
int breakerTwo=51;      //  BREAKER No. 2
int type=0;             // Shows the type of error
int transmitPin=31;    // The digital pin used for sending data to the transmitter
int baudRate = 1000;   // The baud rate for the radio frequency communication.
int trigPin = 32;     //  Defining the pin for exciting the ultrasonic source
int echoPin = 33;     // the sensing pin for  the ultrasonic sensor
const double tTime = 1000000;   // Defining time for the temperature calculation
int tMes=0;         // Is temperature calculation initiated?
double tempCurrent;
/******************************************************/


void setup() 
{
   Serial.begin(9600);      // Setting the baud rate for the serial communication when accessing the controller on-site
   d.begin(16,2);           // Starting the LCD
   emon.voltage(1,581,1);   // Calibration of the voltage for the proper working of the voltage sensor
   emon.current(0,7);       // Calibration of the current parameters for the proper working of the current sensor
   pinMode(breakerOne,OUTPUT);
   pinMode(breakerTwo,OUTPUT);
  

   /***** AS THE SYSTEM IS INTIALLY HEALTHY, SET ALL THE BREAKERS INTO ACTIVE MODE******/
   digitalWrite(breakerOne,HIGH);     
   digitalWrite(breakerTwo,HIGH);
   delay(200);
   /**********************************************************************************/

   /***************Defining the pin modes of the level detection circuit****************/ 
   pinMode(trigPin,OUTPUT);     // Trigger pin for sending out pulses
   pinMode(echoPin,INPUT);      // Echo pin for sensing the echo sent out by the ultrasonic source
   /***********************************************************************************/

   /***********************************For the initiation of the temperature sensor************/ 
   sensors.begin();
   /*******************************************************************************************/

   /********************************For the initiation of the TXN****************************/
   SetupRFDataTxnLink(transmitPin,baudRate);   // Defining the data pin and the baud rate for the TXN
   /*****************************************************************************************/
}

/*********************OPENING THE BREAKERS******************************/
void openAll()
{
  
  /*************AS THE SYSTEM IS OVERLOADED, BREAK ALL THE CONNECTIONS********************/
  digitalWrite(breakerOne,LOW);         
  digitalWrite(breakerTwo,LOW);                       
  /*************************************************************************************/ 
  d.begin(16,2);                
  d.clear();
  d.clear();
  d.setCursor(0,0);
  d.print("OVERLOAD!!!");
  d.setCursor(0,1);
  d.print("BREAKED!!!");
  Serial.print("Stat : ");
  Serial.println(stat);
  Serial.print("Type : ");
  Serial.println(type);
  while(1);
}
/************************************************************************/
void loop()
{ 
  if(tMes==0)
  {
    sensors.requestTemperatures();
    tempCurrent=micros();
    tMes=1;
  }
  if (((micros()-tempCurrent)>tTime)&&(tMes==1))
  {
    temperature = sensors.getTempCByIndex(0);
    tMes=0;
  }
  /****************CALCULATING ELECTRICAL PARAMETERS****************/
  emon.calcVI(20,2000);                 // Sensing of Electrical parameters
  pF = emon.powerFactor;                // taking in power factor value
  pF=pF*(-1);
  voltage=emon.Vrms;                    // taking in voltage value
  current=emon.Irms;                    // taking in current value
  if(voltage<1)                         // Low voltage values cause erraneous reading of power factor
    pF=1;
  if ((micros()-tempCurrent)>tTime&&(tMes==1))
  {
    temperature = sensors.getTempCByIndex(0);
    tMes=0;
  }
  Serial.print("Current : ");
  Serial.println(current);
  Serial.print("Voltage : ");
  Serial.println(voltage);
  Serial.print("Stat : ");
  Serial.println(stat);
  Serial.print("Type :");
  Serial.println(type);
  Serial.print("Temperature : ");
  Serial.println(temperature);
  Serial.print("Oil Level :");
  Serial.println(level);
  S=voltage*current;                    // Calculating the Apparent power
  P=voltage*current*pF;                 // Calculating the Real power
  Q=sqrt(pow(S,2)-pow(P,2));            // Calculating the Reactive power
  /*******************************************************************/

  if((micros()-start>Time)&&(stat==1))
    openAll();
  if ((micros()-tempCurrent)>tTime&&(tMes==1))
  {
    temperature = sensors.getTempCByIndex(0);
    tMes=0;
  }

  /**************************Transmission of Data************************/
  SetupRFDataTxnLink(transmitPin,baudRate);
  float outArray[RF_DATA_ARRAY_SIZE];
  outArray[0] = current;      // Transmitting the voltage value
  outArray[1] = voltage;      // Transmitting the current value
  outArray[2] = pF;           // Transmitting the power factor
  outArray[3] = temperature;  // Transmitting the oil temperature
  outArray[4] = level;        // Transmitting the oil level
  outArray[5]=1;
  union RFData outDataSeq;
  EncodeRFData(outArray,outDataSeq);
  TransmitRFData(outDataSeq);
  SetupRFDataTxnLink(transmitPin,9600);
  /*********************************************************************/

   emon.calcVI(20,2000);                 // Sensing of Electrical parameters
   current=emon.Irms;                    // taking in current value
  /******************************Temperature Sensing*********************/
  if(tMes==0)
  {
    sensors.requestTemperatures();
    tempCurrent=micros();
    tMes=1;
  }
  
  if ((micros()-tempCurrent)>tTime&&(tMes==1))
  {
    temperature = sensors.getTempCByIndex(0);
    tMes=0;
  }
  /************************************************************************/
    
  /*******************Sensing the Oil level*******************************/
  double duration;
  digitalWrite(trigPin,LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin,HIGH);
  delayMicroseconds(10);
  if((micros()-start>Time)&&(stat==1))
    openAll();
  digitalWrite(trigPin,LOW);
  duration=pulseIn(echoPin,HIGH);
  level=duration/29/2;
  if ((micros()-tempCurrent)>tTime&&(tMes==1))
  {
    temperature = sensors.getTempCByIndex(0);
    tMes=0;
  }
  /************************************************************************/

  
  if((micros()-start>Time)&&(stat==1))
    openAll();

  /********************************DISPLAY IN LCD***************************/
  d.begin(16,2);
  d.clear();
  d.setCursor(0,0);
  d.print("I ");
  d.setCursor(2,0);
  d.print(current);
  d.setCursor(7,0);
  d.print("PF ");
  d.setCursor(10,0);
  d.print(pF);
  d.setCursor(0,1);
  d.print("V ");
  d.setCursor(2,1);
  d.print(voltage);
  d.setCursor(7,1);
  d.print("P ");
  d.setCursor(10,1);
  d.print(P);
  if ((micros()-tempCurrent)>tTime&&(tMes==1))
  {
    temperature = sensors.getTempCByIndex(0);
    tMes=0;
  }
  /******************************************************************************************/

  if((micros()-start>Time)&&(stat==1))
    openAll();
  if ((micros()-tempCurrent)>tTime&&(tMes==1))
  {
    temperature = sensors.getTempCByIndex(0);
    tMes=0;
  }
  
  /*****************************TAKING DECISION*************************/
  if(stat==0)
  {
    if(current>2&&current<=2.5)
    {
      Time=120000000;
      stat=1;
      start=micros();
      Serial.print("5");
      type=1;
    }
    else if(current>2.5&&current<=3)
    {
      Time=60000000;
      stat=1;
      start=micros();
      Serial.print("6");
      type=2;
    }
    else if(current>3&&current<3.7)
    {
      Time=30000000;
      stat=1;
      start=micros();
      Serial.print("10");
      type=3;
    }
    else if(current>3.7)
    {
      type=4;
      openAll();
    }
    if ((micros()-tempCurrent)>tTime&&(tMes==1))
    {
      temperature = sensors.getTempCByIndex(0);
      tMes=0;
    }
  }
  if((micros()-start>Time)&&(stat==1))
    openAll();
  else if(stat==1&&type==1)
  {
    if(current>2.5)
    {
      if(current<=3)
      {
        type=2;
        Time=60000000;
      }
      else if(current>3&&current<3.7)
      {
        type=3;
        Time=30000000;
      }
      else
      {
        type=4;
        openAll();  
      }
    }
    else if(current<2)
    {
      stat=0;
      type=0;
    }
  }
  else if(stat==1&&type==2)
  {
    if(current>3.7)
    {
      type=4;
      openAll();
    }
    else if(current<3.7&&current>3)  
    {
      type=3;
      Time=30000000;
    }
    else if(current<2.5)
    {
      if(current>2)
      {
        type=1;
        Time=120000000;
      }
      else
      {
        type=0;
        stat=0;
      }
    }
  }
  else if(stat==1&&type==3)
  {
    if(current<2)
    {
      type=0;
      stat=0;
    }
    else if(current>2&&current<2.5)
    {
      type=1;
      Time=120000000;
    }
    else if(current>2.5&&current<3)
    {
      type=2;
      Time=60000000;
    }
    else if(current>3.7)
    {
      type=4;
      openAll();
    }
  }
  if((micros()-start>Time)&&(stat==1))
    openAll();
  if ((micros()-tempCurrent)>tTime&&(tMes==1))
  {
    temperature = sensors.getTempCByIndex(0);
    tMes=0;
  }
}
