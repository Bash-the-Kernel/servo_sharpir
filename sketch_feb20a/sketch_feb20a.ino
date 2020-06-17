 
#include <Servo.h>

#include "ESP32_MailClient.h"
#include "SD.h"

#define WIFI_SSID "Internet machine"
#define WIFI_PASSWORD "0e43f279b464"
static const int servoPin = 14;

const int trigPin = 27;
const int echoPin = 12;

long duration; 
int dist;


int dist_list_1 [36] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
int dist_list_2 [36] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

//The Email Sending data object contains config and data to send
SMTPData smtpData;

//Callback function to get the Email sending status
void sendCallback(SendStatus info);
Servo servo1;

int angle =0;
int angleStep = 5;
bool clockwise = true;
int empty_spots = 0;
bool is_empty = true;
int angleMin =0;
int angleMax = 180;


void setup()
{
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input
  
  Serial.begin( 9600 );
  servo1.attach(servoPin);
  Serial.println();

  Serial.print("Connecting to AP");

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(200);
  }

  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println();


  Serial.println("Mounting SD Card...");

  if (SD.begin()) // MailClient.sdBegin(14,2,15,13) for TTGO T8 v1.7 or 1.8
  {

    Serial.println("Preparing attach file...");

    File file = SD.open("/text_file.txt", FILE_WRITE);
    file.print("Hello World!\r\nHello World!");
    file.close();

    file = SD.open("/binary_file.dat", FILE_WRITE);

    static uint8_t buf[512];

    buf[0] = 'H';
    buf[1] = 'E';
    buf[2] = 'A';
    buf[3] = 'D';
    file.write(buf, 4);

    size_t i;
    memset(buf, 0xff, 512);
    for (i = 0; i < 2048; i++)
    {
      file.write(buf, 512);
    }

    buf[0] = 'T';
    buf[1] = 'A';
    buf[2] = 'I';
    buf[3] = 'L';
    file.write(buf, 4);

    file.close();
  }
  else
  {
    Serial.println("SD Card Monting Failed");
  }

  Serial.println();


  Serial.println("Sending email...");

  //Set the Email host, port, account and password




}

void loop()
{
  if(angle == angleMax)
  {
    clockwise = false;
  }
  if(angle == angleMin)
  {
    clockwise = true;
  }

  if(clockwise == false)
  {
    // Clears the trigPin
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    // Sets the trigPin on HIGH state for 10 micro seconds
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    // Reads the echoPin, returns the sound wave travel time in microseconds
    duration = pulseIn(echoPin, HIGH);
    // Calculating the dist
    dist= duration*0.034/2;
    angle -= angleStep;
    Serial.println( dist );
    servo1.write(angle);
    dist_list_1[angle/5] = dist;
    empty_spots = 0;
    for(int i = 0; i < 37; i++)
    {
      if(dist_list_1[i] == 0){
        empty_spots++;
      }
    }
    for(int i = 0; i < 37; i++)
    {
      if(dist_list_2[i] == 0){
        empty_spots++;
      }
    }
    if(empty_spots == 0)
    {
      if(dist_list_2[angle/5] > dist_list_1[36-angle/5]+100 || \
      dist_list_2[angle/5] < dist_list_1[36-angle/5]-100 || \
      dist_list_2[angle/5] > dist_list_1[35-angle/5]+100 || \
      dist_list_2[angle/5] < dist_list_1[35-angle/5]-100 || \
      dist_list_2[angle/5] > dist_list_1[37-angle/5]+100 || \
      dist_list_2[angle/5] < dist_list_1[37-angle/5]-100 )
      {
          smtpData.setLogin("smtp.gmail.com", 587, "esp32.radar@gmail.com", "Golodi45");

          //For library version 1.2.0 and later which STARTTLS protocol was supported,the STARTTLS will be 
          //enabled automatically when port 587 was used, or enable it manually using setSTARTTLS function.
          //smtpData.setSTARTTLS(true);

          //Set the sender name and Email
          smtpData.setSender("ESP32", "esp32.radar@gmail.com");

          //Set Email priority or importance High, Normal, Low or 1 to 5 (1 is highest)
          smtpData.setPriority("High");

          //Set the subject
          smtpData.setSubject("ESP32 SMTP Mail Sending Test");

          //Set the message - normal text or html format
          smtpData.setMessage("<div style=\"color:#ff0000;font-size:20px;\">There is a disturbance in your room!</div>", true);

          //Add recipients, can add more than one recipient
          smtpData.addRecipient("jackcharlie258@gmail.com");



          //Add attachments, can add the file or binary data from flash memory, file in SD card
          //Data from internal memory

          //Add attach files from SD card
          //Comment these two lines, if no SD card connected
          //Two files that previousely created.
          smtpData.addAttachFile("/binary_file.dat");
          smtpData.addAttachFile("/text_file.txt");


          //Add some custom header to message
          //See https://tools.ietf.org/html/rfc822
          //These header fields can be read from raw or source of message when it received)
          smtpData.addCustomMessageHeader("Date: Sat, 10 Aug 2019 21:39:56 -0700 (PDT)");
          //Be careful when set Message-ID, it should be unique, otherwise message will not store
          //smtpData.addCustomMessageHeader("Message-ID: <abcde.fghij@gmail.com>");

          //Set the storage types to read the attach files (SD is default)
          //smtpData.setFileStorageType(MailClientStorageType::SPIFFS);
          smtpData.setFileStorageType(MailClientStorageType::SD);

          smtpData.setSendCallback(sendCallback);
          //Start sending Email, can be set callback function to track the status
          if (!MailClient.sendMail(smtpData))
          {
            Serial.println("Error sending Email, " + MailClient.smtpErrorReason());
          }
          //delay(24000);//wait four minutes
          //Clear all data from Email object to free memory
          smtpData.empty();
      }
    }
  }
  else
  {
    // Clears the trigPin
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    // Sets the trigPin on HIGH state for 10 micro seconds
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    // Reads the echoPin, returns the sound wave travel time in microseconds
    duration = pulseIn(echoPin, HIGH);
    // Calculating the dist
    dist = duration*0.034/2;
    angle += angleStep;
    Serial.println( dist );
    servo1.write(angle);
    dist_list_2[angle/5] = dist;
    empty_spots = 0;
    for(int i = 0; i < 37; i++)
    {
      if(dist_list_1[i] == 0){
        empty_spots++;
      }
    }
    for(int i = 0; i < 37; i++)
    {
      if(dist_list_2[i] == 0){
        empty_spots++;
      }
    }
    if(empty_spots == 0)
    {
      if(dist_list_2[angle/5] > dist_list_1[36-angle/5]+100 || \
      dist_list_2[angle/5] < dist_list_1[36-angle/5]-100 || \
      dist_list_2[angle/5] > dist_list_1[35-angle/5]+100 || \
      dist_list_2[angle/5] < dist_list_1[35-angle/5]-100 || \
      dist_list_2[angle/5] > dist_list_1[37-angle/5]+100 || \
      dist_list_2[angle/5] < dist_list_1[37-angle/5]-100)
      {
        
          smtpData.setLogin("smtp.gmail.com", 587, "esp32.radar@gmail.com", "Golodi45");

          //For library version 1.2.0 and later which STARTTLS protocol was supported,the STARTTLS will be 
          //enabled automatically when port 587 was used, or enable it manually using setSTARTTLS function.
          //smtpData.setSTARTTLS(true);

          //Set the sender name and Email
          smtpData.setSender("ESP32", "esp32.radar@gmail.com");

          //Set Email priority or importance High, Normal, Low or 1 to 5 (1 is highest)
          smtpData.setPriority("High");

          //Set the subject
          smtpData.setSubject("ESP32 SMTP Mail Sending Test");

          //Set the message - normal text or html format
          smtpData.setMessage("<div style=\"color:#ff0000;font-size:20px;\">There is a disturbance in your room!</div>", true);

          //Add recipients, can add more than one recipient
          smtpData.addRecipient("jackcharlie258@gmail.com");



          //Add attachments, can add the file or binary data from flash memory, file in SD card
          //Data from internal memory

          //Add attach files from SD card
          //Comment these two lines, if no SD card connected
          //Two files that previousely created.
          smtpData.addAttachFile("/binary_file.dat");
          smtpData.addAttachFile("/text_file.txt");


          //Add some custom header to message
          //See https://tools.ietf.org/html/rfc822
          //These header fields can be read from raw or source of message when it received)
          smtpData.addCustomMessageHeader("Date: Sat, 10 Aug 2019 21:39:56 -0700 (PDT)");
          //Be careful when set Message-ID, it should be unique, otherwise message will not store
          //smtpData.addCustomMessageHeader("Message-ID: <abcde.fghij@gmail.com>");

          //Set the storage types to read the attach files (SD is default)
          //smtpData.setFileStorageType(MailClientStorageType::SPIFFS);
          smtpData.setFileStorageType(MailClientStorageType::SD);
          smtpData.setSendCallback(sendCallback);
          //Start sending Email, can be set callback function to track the status
          if (!MailClient.sendMail(smtpData))
          {
            Serial.println("Error sending Email, " + MailClient.smtpErrorReason());
          }
          //delay(240000); // wait four minutes
          //Clear all data from Email object to free memory
          smtpData.empty();
      }
    }
  }
  delay(100);
}



//Callback function to get the Email sending status
void sendCallback(SendStatus msg)
{
  //Print the current status
  Serial.println(msg.info());

  //Do something when complete
  if (msg.success())
  {
    Serial.println("----------------");
  }
}
