#include <SharpIR.h>

#include <Servo.h>

#include <Wifi.h>

const char* ssid     = "yourssid";
const char* password = "yourpasswd";


#define SHARPIR 12
static const int servoPin = 14;

Servo servo1;

int angle =0;
int angleStep = 5;

int angleMin =0;
int angleMax = 180;


void setup()
{
  Serial.begin( 115200 );
  servo1.attach(servoPin);
}

void loop()
{
  float volts = analogRead(SHARPIR)*0.0048828125;
  int distance = 13*pow(volts, -1);
  Serial.println(distance);
  Serial.println( distance ); //Print the value to the serial monitor
  
    for(int angle = 0; angle <= angleMax; angle +=angleStep) {
        servo1.write(angle);
        delay(20);
    }

    for(int angle = 180; angle >= angleMin; angle -=angleStep) {
        servo1.write(angle);
        delay(20);
    }
}
