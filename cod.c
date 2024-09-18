#include <Servo.h>
Servo myservo;  // creeaza obiect servo

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Adresele I2C standard pentru diferite dimensiuni ale ecranului LCD
#define I2C_ADDR 0x27 // Adresa I2C a modulului LCD

// Definirea numărului de coloane și rânduri ale LCD-ului
#define LCD_COLS 12
#define LCD_ROWS 2

// Inițializarea obiectului LCD cu adresa I2C specificată mai sus
LiquidCrystal_I2C lcd(I2C_ADDR, LCD_COLS, LCD_ROWS);

//PINS:
//Ultrasonic sensor 1
const int sensor1trigPin = 3;
const int sensor1echoPin = 4;
//Ultrasonic sensor 2
const int sensor2trigPin = 5;
const int sensor2echoPin = 6;

const long intervalCoborareBariera = 3000; // Intervalul de timp în milisecunde pt coborarea barierei


void setup() {
  
  Serial.begin(9600);
  //Servo
  myservo.attach(9);  // (pin, min, max) 
  //UltraSensor 1
  pinMode(sensor1trigPin, OUTPUT);
  pinMode(sensor1echoPin, INPUT);
  //UltraSensor 2
  pinMode(sensor2trigPin, OUTPUT);
  pinMode(sensor2echoPin, INPUT);
  
  //LCD
  // initializare ecran lcd
  lcd.init();
  // turn on the backlight
  lcd.backlight();
  
  //LED-URI
  pinMode ( 13, OUTPUT); 
  pinMode ( 7, OUTPUT);

  //initial bariera este inchisa
  myservo.write(0);
}

void functieDelayMicroseconds(unsigned int microseconds) {
  unsigned long start = micros();
  while (micros() - start < microseconds) {
    //nothing
  }
}
  

// Sensor ultrasonic 1
long U1()
{
  long duration, distance;
  // Clears the trigPin
  digitalWrite(sensor1trigPin, LOW);
  
  functieDelayMicroseconds(2);
  //Seteaza trigPin in starea HIGH pt 10 microssecunde
  digitalWrite(sensor1trigPin, HIGH);
  functieDelayMicroseconds(10);
  digitalWrite(sensor1trigPin, LOW);
  
  duration = pulseIn(sensor1echoPin, HIGH);

  distance = duration * 0.034 / 2;
  
  return distance;
}

long U2()
{
  long duration, distance;
  // Clears the trigPin
  digitalWrite(sensor2trigPin, LOW);
  
  functieDelayMicroseconds(2);
  //Seteaza trigPin in starea HIGH pt 10 microssecunde
  digitalWrite(sensor2trigPin, HIGH);
  functieDelayMicroseconds(10);
  digitalWrite(sensor2trigPin, LOW);
  
  duration = pulseIn(sensor2echoPin, HIGH);
  // calculam distanta
  distance = duration * 0.034 / 2;
  
  return distance;
}

// Variabile
int freeParkingSpots = 4; //consideram ca parcarea are initial 4 locuri libere si totodata 4 locuri in total
unsigned long previousMillisBariera = 0; // Pentru a urmări timpul anterior cand s-a ridicat bariera
unsigned long previousMillisLCD = 0; 
int flag1 = 0;
int flag2 = 0;

void loop() {

  unsigned long currentMillis = millis();
  
  int distance1 = U1(); //distanta fata de senzorul 1 la intrarea in parcare
  int distance2 = U2(); //distanta fata de senzorul 2 la iesirea din parcare

  if(distance1 < 10 && flag1==0){ //daca senzorul de la intrare detecteaza masina 
    if(freeParkingSpots > 0){ //daca mai sunt locuri de parcare libere
      flag1 = 1;
      if(flag2==0){
        myservo.write(90);
        previousMillisBariera = currentMillis;
        freeParkingSpots = freeParkingSpots-1;
      }
    }else{
        lcd.setCursor(0, 0);
        lcd.clear();
        lcd.print(" Parking is full ");
        previousMillisLCD = currentMillis; // Start timer pt LCD 
    }
  }

  if(distance2 < 10 && flag2==0){ //daca senzorul de la iesire detecteaza masina 
    flag2 = 1;
    if(flag1 == 0){
      myservo.write(90);
      previousMillisBariera = currentMillis;
      if(freeParkingSpots < 4)
        freeParkingSpots = freeParkingSpots + 1;
    }
  }

  if(flag1==1 && flag2==1 && (currentMillis - previousMillisBariera >= intervalCoborareBariera)){ //daca ambii senzori au detectat o masina, inseamna ca ea a trecut de ambii,
                                                                                                  //iar dupa 3 secunde, bariera se inchide
    myservo.write(0);
    flag1=0;
    flag2=0;
  }

  if (currentMillis - previousMillisLCD >= 1000) {
    previousMillisLCD = currentMillis;
    // write on the top row
    lcd.setCursor(0, 0);
    lcd.print("Available spots:");
    // write on the bottom row
    lcd.setCursor(0, 1);
    lcd.print(freeParkingSpots);
  }
  
  //leduri
  if(freeParkingSpots > 0){ //daca mai sunt locuri libere, led-ul galben se aprinde
    digitalWrite (13, LOW);  
  	digitalWrite (7, HIGH);   
  }else{ //daca nu mai sunt locuri libere in parcare, ledul rosu se aprinde
    digitalWrite (13, HIGH);   
  	digitalWrite (7, LOW);  
  }
}

