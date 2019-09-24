#include<LiquidCrystal.h>
LiquidCrystal lcd1(37, 27, 33, 35, 31, 29);
LiquidCrystal lcd2(47, 49, 45, 43, 41, 39);
int R1=26;
int R2=28;
int R3=30;
int R4=32;
int R6=53;
int R7=52;
int R8=51;
int R1on = 40;
int R1off = 36;
int R2on = 42;
int R2off =38;
int R3on = 48;
int R3off =44;
int R4on = 50;
int R4off = 46;
//float current = A1; 
float overload = 0;
int Time = 0;
int i=0;

void setup() 
{
  pinMode(R1,OUTPUT);
  pinMode(R2,OUTPUT);
  pinMode(R3,OUTPUT);
  pinMode(R4,OUTPUT);
  pinMode(R6,OUTPUT);
  pinMode(R7,OUTPUT);
  pinMode(R8,OUTPUT);
  pinMode(R1on,OUTPUT);
  pinMode(R1off,OUTPUT);
  pinMode(R2on,OUTPUT);
  pinMode(R2off,OUTPUT);
 pinMode(R3on,OUTPUT);
 pinMode(R3off,OUTPUT);
 pinMode(R4on,OUTPUT);
  pinMode(R4off,OUTPUT); 
  
  Serial.begin(9600);
  lcd1.begin(16,2);
  lcd1.print("volt curt pow");
  lcd2.begin(16,2);
  lcd2.print("OVERLOAD PROTECT!");
  //lcd1.clear();
  lcd2.clear();
}
int current=0;
void loop() 
{
  digitalWrite(R1,HIGH);
  digitalWrite(R2, HIGH);
  digitalWrite(R3, HIGH);
  digitalWrite(R4, HIGH);
  digitalWrite(R8,HIGH);
  digitalWrite(R6, HIGH);
  digitalWrite(R7, HIGH);
  digitalWrite(R1on, HIGH);
  digitalWrite(R2on, HIGH);
  digitalWrite(R3on, HIGH);
  digitalWrite(R4on, HIGH);

 float a, voltage, b, power, pf=1, overload, i, Time;
 char cur;
 a = analogRead(A0);
 //b = analogRead(A1);
 voltage = map(a,0,1023,0,202);
 //current = map(b,0,1023,0,1.98);
 
 power = current*voltage*pf;
 if(Serial.available()>=0)
 {
    cur = Serial.read();
 }
 if(cur=='t')
  current=4;
 else if(cur=='h')
  current=1;
 if(current < 1.98 || current == 1.98)
{
  lcd2.setCursor(0,0);
  lcd2.print("NO overload");
  lcd2.setCursor(0,1);
  lcd2.print("System healthy");
}

else
{
  
  overload = ((current - 0.33)/0.33)*100;
  
  if (overload > 0 && overload <= 15)
  {
    Time = 1800;
  }
  
  else if(overload > 15 && overload <= 30)
  {
    Time = 1200;  
  }
  
  else if(overload > 30 && overload <= 45)
  {
    Time = 600;  
  }
  
  else if(overload > 45 && overload <= 60)
  {
    Time = 300;  
  }
  
  else if(overload > 60 && overload <= 100)
  {
    Time = 150;  
  }
  
  else if(overload > 100 && overload <= 150)
  {
    Time = 60;  
  }
  
  else if(overload > 150 && overload <= 200)
  {
    Time = 5;  
  }
  
  else if(overload > 200)
  {
    Time = 1;  
  }
  lcd2.setCursor(0,0);
  lcd2.print("Overload % :");
  lcd2.setCursor(0,12);
  lcd2.print(overload);
  
 Serial.print(a);
 Serial.print(b);
 delay(500);
 if(1)
 {
 lcd1.setCursor(0,1);
 lcd1.print(voltage);
 lcd1.setCursor(0,6);
 lcd1.print(current);
 lcd1.setCursor(0,11);
 lcd1.print(power);
 }
 for (i=0; i<Time; i)
  {
    delay(1000);
    if (Time == 1)
    {
      digitalWrite(R4on, LOW);
      digitalWrite(R4off, HIGH);
      digitalWrite(R6, LOW);
      delay(5000);
      lcd2.clear();
      lcd2.print("Load2 cut off!");
      while(1);
      break;
    }
    else 
    {
      lcd2.setCursor(1,0);
      lcd2.print(Time);
    }
    if (current < 1.98)
    {
       lcd2.setCursor(0,0);
       lcd2.print("system restored");
       break;
    }
    else
    {
      delay(1);
    }
    if (current > 6)
    {
      digitalWrite(R1, LOW);
      digitalWrite(R2, LOW);
      digitalWrite(R3, LOW);
      digitalWrite(R4, LOW);
      digitalWrite(R1on, LOW);
      digitalWrite(R2on, LOW);
      digitalWrite(R1off, HIGH);
      digitalWrite(R2off, HIGH);
      lcd2.setCursor(0,0);
      lcd2.print("EXCESSIVE LOAD");
      break; 
    }
     Time = Time -1;
  }
}
}
