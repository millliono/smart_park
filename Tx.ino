#include <SPI.h>
#include <RF22.h>
#include <RF22Router.h>
#include <OneWire.h>

#define MY_ADDRESS 7
#define DESTINATION_ADDRESS 5
#define FREQUENCY 431.0

#define TRIGPIN 4
#define ECHOPIN 5
#define DS18S20_Pin 6


RF22Router rf22(MY_ADDRESS);
OneWire ds(DS18S20_Pin);

int carParked = 0;
long randNumber;
boolean successful_packet = false;
int max_delay=700;


// defines variables
long duration;
float distance;
float temperature;
int numOfFailures;
// int light;

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

  randomSeed(5);// (μία μόνο φορά μέσα στην setup)
}

void loop() {
  distance = sonicDistance();
  temperature = getTemp();
  // light = analogRead(A0);
  Serial.println(distance);
  Serial.println(temperature);
  // Serial.println(light);
  if (distance<90){
    carParked = 1;
  }
  else{
    carParked = 0;
  }

  // if (light<70){
  //   light = 1;
  // }
  // else{
  //   light = 0;
  // }

  char data_read[RF22_ROUTER_MAX_MESSAGE_LEN];
  uint8_t data_send[RF22_ROUTER_MAX_MESSAGE_LEN];
  memset(data_read, '\0', RF22_ROUTER_MAX_MESSAGE_LEN);
  memset(data_send, '\0', RF22_ROUTER_MAX_MESSAGE_LEN);
  sprintf(data_read, "%d,%d", carParked, (int)temperature);
  data_read[RF22_ROUTER_MAX_MESSAGE_LEN - 1] = '\0';
  memcpy(data_send, data_read, RF22_ROUTER_MAX_MESSAGE_LEN);

  successful_packet = false;
  numOfFailures = 0;
  while (!(successful_packet  || numOfFailures > 5))
  {

    if (rf22.sendtoWait(data_send, sizeof(data_send), DESTINATION_ADDRESS) != RF22_ROUTER_ERROR_NONE)
    {
      Serial.println("sendtoWait failed");
      randNumber = random(200,max_delay);
      numOfFailures++;
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
float getTemp(){
  //returns the temperature from one DS18S20 in DEG Celsius

  byte data[12];
  byte addr[8];

  if ( !ds.search(addr)) {
      //no more sensors on chain, reset search
      ds.reset_search();
      return -1000;
  }

  if ( OneWire::crc8( addr, 7) != addr[7]) {
      Serial.println("CRC is not valid!");
      return -1000;
  }

  if ( addr[0] != 0x10 && addr[0] != 0x28) {
      Serial.print("Device is not recognized");
      return -1000;
  }

  ds.reset();
  ds.select(addr);
  ds.write(0x44,1); // start conversion, with parasite power on at the end

  byte present = ds.reset();
  ds.select(addr);
  ds.write(0xBE); // Read Scratchpad


  for (int i = 0; i < 9;  i++  ) { // we need 9 bytes
    data[i] = ds.read();
  }

  ds.reset_search();

  byte MSB = data[1];
  byte LSB = data[0];

  float tempRead = ((MSB << 8) | LSB); //using two's compliment
  float TemperatureSum = tempRead / 16;

  return TemperatureSum;

}