#include<LiquidCrystal.h>

int pump=10;
int sensorValue = A0;
int Real;

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

void setup() 

{
  Serial.begin(9600);
  lcd.begin(16, 2);
  pinMode(pump, OUTPUT);
  pinMode(sensorValue, INPUT);
 }

void loop() 

{
int sensorValue = analogRead(A0);
int Real = map(sensorValue, 1024,0,0,100);
Serial.println(Real);
delay(250);
lcd.setCursor(0,0);
lcd.print("Moisture = %");
lcd.print(Real);
lcd.noDisplay();
delay(0);
lcd.display();
delay(0);

if(Real<50)
  {
    digitalWrite(10, HIGH);
    lcd.setCursor(0,1);
    lcd.print("PUMP IS ON     ");
  }else
  {
    digitalWrite(10, LOW);
    lcd.setCursor(0,1);
    lcd.print("PUMP IS OFF");
  }
}
