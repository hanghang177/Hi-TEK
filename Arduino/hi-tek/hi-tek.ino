#include <Servo.h>
#include <SoftwareSerial.h>

#define weaponmotor 3
#define M1 4
#define E1 5
#define E2 6
#define M2 7
#define leftmotor 8
#define rightmotor 9

Servo weapon;
Servo left;
Servo right;

SoftwareSerial mySerial (10, 11);

String last_received = "";
String buff = "";

int weaponspeed = 0;
int leftspeed = 0;
int rightspeed = 0;

int weaponspeed2 = 0;

long lastmillis = 0;

void rx(){
  while(mySerial.available()){
    byte rd = mySerial.read();
    buff += (char)rd;
    //buff += mySerial.read();
  }
  int commaindex = buff.lastIndexOf(',');
  if(commaindex > -1){
    lastmillis = millis();
    last_received = buff.substring(0,commaindex);
    buff = buff.substring(commaindex+1);
    int lrlen = last_received.length();
    weaponspeed = ((byte)(last_received.charAt(lrlen-1)));
    leftspeed = ((byte)(last_received.charAt(lrlen-3)));
    rightspeed = ((byte)(last_received.charAt(lrlen-2)));
    weaponspeed *= 3.9;
    leftspeed -= 127;
    leftspeed *= 1.9;
    rightspeed -= 127;
    rightspeed *= 1.9;
  }
}

void failsafe(){
  if(millis() - lastmillis > 1000){
    weaponspeed = 0;
    leftspeed = 0;
    rightspeed = 0;
    digitalWrite(13, LOW);
  }else{
    digitalWrite(13, HIGH);
  }
}

void go(){
  if(0 <= weaponspeed <= 1000){
    weapon.writeMicroseconds(1000+weaponspeed);
  }else{
    weapon.writeMicroseconds(1000);
  }
  if(leftspeed >= 0){
    analogWrite(E1, leftspeed);
    digitalWrite(M1, HIGH);
  }else{
    analogWrite(E1, -leftspeed);
    digitalWrite(M1, LOW);
  }
  if(rightspeed >= 0){
    analogWrite(E2, rightspeed);
    digitalWrite(M2, LOW);
  }else{
    analogWrite(E2, -rightspeed);
    digitalWrite(M2, HIGH);
  }
}

void setup() {
  // put your setup code here, to run once:
   Serial.begin(115200);
   for(int i=4;i<=7;i++){
      pinMode(i, OUTPUT);
   }
   pinMode(2, INPUT);
   pinMode(11, OUTPUT);
   mySerial.begin(2400);
   pinMode(13, OUTPUT);
   weapon.attach(weaponmotor);
   weapon.writeMicroseconds(1000);
   left.attach(leftmotor);
   right.attach(rightmotor);
   delay(15000);
}

void loop() {
  // put your main code here, to run repeatedly:
  rx();
  failsafe();
  go();
}
