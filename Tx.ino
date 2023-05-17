#include <SPI.h>
#include <RF22.h>
#include <RF22Router.h>
#define MY_ADDRESS 3
#define DESTINATION_ADDRESS 5
#define FREQUENCY 431.0

#define TRIGPIN 4
#define ECHOPIN 5

RF22Router rf22(MY_ADDRESS);
int carParked = 10;
long randNumber;
boolean successful_packet = false;
int max_delay=700;

// defines variables
long duration;
float distance;


void setup() {
  pinMode(TRIGPIN, OUTPUT); // Sets the TRIGPIN as an Output
  pinMode(ECHOPIN, INPUT); // Sets the ECHOPIN as an Input
  Serial.begin(9600); // Starts the serial communication
  
  if (!rf22.init())
    Serial.println("RF22 init failed");
  if (!rf22.setFrequency(FREQUENCY))
    Serial.println("setFrequency Fail");
  rf22.setTxPower(RF22_TXPOW_20DBM);
  rf22.setModemConfig(RF22::GFSK_Rb125Fd125);

  // Manually define the routes for this network
  rf22.addRouteTo(DESTINATION_ADDRESS, DESTINATION_ADDRESS);
  carParked = 5;
  randomSeed(carParked);// (μία μόνο φορά μέσα στην setup)
}

void loop() {
  // delay(100);
  distance = sonicDistance();
  Serial.println(distance);
  if (distance<90){
    carParked = 1;
  }
  else{
    carParked = 0;
  }

  char data_read[RF22_ROUTER_MAX_MESSAGE_LEN];
  uint8_t data_send[RF22_ROUTER_MAX_MESSAGE_LEN];
  memset(data_read, '\0', RF22_ROUTER_MAX_MESSAGE_LEN);
  memset(data_send, '\0', RF22_ROUTER_MAX_MESSAGE_LEN);
  sprintf(data_read, "%d", carParked);
  data_read[RF22_ROUTER_MAX_MESSAGE_LEN - 1] = '\0';
  memcpy(data_send, data_read, RF22_ROUTER_MAX_MESSAGE_LEN);

  successful_packet = false;
  while (!successful_packet)
  {

    if (rf22.sendtoWait(data_send, sizeof(data_send), DESTINATION_ADDRESS) != RF22_ROUTER_ERROR_NONE)
    {
      Serial.println("sendtoWait failed");
      randNumber=random(200,max_delay);
      Serial.println(randNumber);
      delay(randNumber);
    }
    else
    {
      successful_packet = true;
      Serial.println("sendtoWait Succesful");
      delay(1000);
    }
  }  

}

float sonicDistance(){
  // Clears the TRIGPIN
  digitalWrite(TRIGPIN, LOW);
  delayMicroseconds(2);
  // Sets the TRIGPIN on HIGH state for 10 micro seconds
  digitalWrite(TRIGPIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGPIN, LOW);
  // Reads the ECHOPIN, returns the sound wave travel time in microseconds
  duration = pulseIn(ECHOPIN, HIGH);
  // Calculating the distance
  distance = duration * 0.034 / 2;
  // Prints the distance on the Serial Monitor

  return distance;
}