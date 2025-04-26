/*A basic 7 channel transmitter using the nRF24L01 module and STM32.*/
/* Like, share and subscribe, ELECTRONOOBS */
/* http://www.youtube/c/electronoobs */

/* First we include the libraries. Download it from 
   my webpage if you don't have the NRF24 library */
 
#include <SPI.h>
#include <nRF24L01.h>             //Downlaod it here: https://www.electronoobs.com/eng_arduino_NRF24.php
#include <RF24.h>              


//Inputs/outputs
#define throttle_pin PA0
#define yaw_pin PA1
#define roll_pin PA2
#define pitch_pin PA3
#define aux1_pin PB8
#define aux2_pin PB3
#define pot_pin PA4


const uint64_t pipeOut = 0xE8E8F0F0E1LL; //IMPORTANT: The same as in the receiver!!!
RF24 radio(PB4, PB5); // select  CSN  pin

/*The sizeof this struct should not exceed 32 bytes. This gives us up to 32 of 8 bits channals.
I will send two bytes for each channel*/

struct MyData {
  long throttle;
  long yaw;
  long roll;
  long pitch;
  bool AUX1;            //This is a digital channel (1000 or 2000), so we don't need precision
  bool AUX2;
};

MyData data;

void resetData() 
{
  //This are the start values of each channal
  //Throttle is 0 in order to stop the motors (in case of a drone)
  //2048 is the middle value of the 12ADC.    
  data.throttle = 0;  
  data.yaw = 2048;  
  data.pitch = 2048;  
  data.roll = 2048;  
  data.AUX1 = 0;
  data.AUX2 = 0;
}


void setup() {
  pinMode(throttle_pin, INPUT);
  pinMode(yaw_pin, INPUT);
  pinMode(roll_pin, INPUT);
  pinMode(pitch_pin, INPUT);
  pinMode(aux1_pin, INPUT_PULLUP);
  pinMode(aux2_pin, INPUT_PULLUP);
  pinMode(pot_pin, INPUT);
  Serial.begin(9600);

  //Start everything up
  radio.begin();        
  radio.setAutoAck(false);                    // Ensure autoACK is enabled
  radio.setChannel(0x2F);
  radio.setPALevel(RF24_PA_MAX);  
  radio.setDataRate(RF24_2MBPS);
  radio.openWritingPipe(pipeOut);  
  resetData();
  Serial.println("TX ready");
}

void loop() {
  long throttle =   analogRead(throttle_pin);    //It will make a read from 0 to 4095
  long yaw =        analogRead(yaw_pin);  
  long roll =       analogRead(roll_pin); 
  long pitch =      analogRead(pitch_pin);  
  bool aux_1 =      digitalRead(aux1_pin);  
  bool aux_2 =      digitalRead(aux2_pin);  

  Serial.println("va0.txt=\"t");
  Serial.println(map(throttle,0,4096,1000,2000));

  Serial.println("y");
  Serial.println(map(yaw,0,4096,1000,2000));

  Serial.println("r");
  Serial.println(map(roll,0,4096,1000,2000));

  Serial.println("p");
  Serial.println(map(pitch,0,4096,1000,2000));

  Serial.println("s");
  Serial.println(1000*(aux_1+1));

  Serial.println("w");
  Serial.println(1000*(aux_2+1));
  Serial.println("\"");


  //We need to send 3 full bytes after each value
  Serial.write(0xff);
  Serial.write(0xff);
  Serial.write(0xff);



  data.throttle = throttle;  
  data.yaw = yaw;  
  data.pitch = pitch;  
  data.roll = roll;  
  data.AUX1 = aux_1;
  data.AUX2 = aux_2;

  radio.write(&data, sizeof(MyData)); 
  delay(1000);
}
