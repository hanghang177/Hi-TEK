#include <Servo.h>
#include <SoftwareSerial.h>

#define weaponmotor 3
#define M1 4
#define E1 5
#define E2 6
#define M2 7
#define leftmotor 8
#define rightmotor 9
#define rxPin 10
#define txPin 11

#define timeout 1000

#define invertible 1

Servo weapon;
Servo left;
Servo right;

SoftwareSerial mySerial (rxPin, txPin);

String last_received = "";
String buff = "";

int weaponspeed = 0;
int leftspeed = 0;
int rightspeed = 0;

long lastmillis = 0;

void rx(){
  while(mySerial.available()){
    byte rd = mySerial.read();
    buff += (char)rd;
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
    if (invertible == 1) {
      weaponspeed = map(weaponspeed, 1, 255, -500, 500);
    }else{
      weaponspeed = map(weaponspeed, 1, 255, 0, 1000);
    }
    leftspeed -= 127;
    leftspeed *= 1.9;
    rightspeed -= 127;
    rightspeed *= 1.9;
  }
}

void failsafe(){
  if(millis() - lastmillis > timeout){
    weaponspeed = 0;
    leftspeed = 0;
    rightspeed = 0;
    digitalWrite(13, LOW);
  }else{
    digitalWrite(13, HIGH);
  }
}

void go(){
  if(invertible == 1){
    if((-500 <= weaponspeed) && (weaponspeed <= 500)){
      if((-100 <= weaponspeed) && (weaponspeed <= 100)){
        weapon.writeMicroseconds(1500);
      }else{
        weapon.writeMicroseconds(1500 + weaponspeed);
      }
    }else{
      weapon.writeMicroseconds(1500);
    }
  }else{
    if((0 <= weaponspeed) && (weaponspeed <= 1000)){
      if(weaponspeed <= 100){
        weapon.writeMicroseconds(1000);
      }else{
        weapon.writeMicroseconds(1000 + weaponspeed);
      }
    }else{
      weapon.writeMicroseconds(1000);
    }
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
  pinMode(2, INPUT);
  for(int i=4;i<=7;i++){
    pinMode(i, OUTPUT);
  }
  pinMode(11, OUTPUT);
  pinMode(13, OUTPUT);
  mySerial.begin(2400);
  weapon.attach(weaponmotor);
  if(invertible == 1){
    weapon.writeMicroseconds(1500);
  }else{
    weapon.writeMicroseconds(1000);
  }
  left.attach(leftmotor);
  right.attach(rightmotor);
}

void loop() {
  // put your main code here, to run repeatedly:
  rx();
  failsafe();
  go();
}
