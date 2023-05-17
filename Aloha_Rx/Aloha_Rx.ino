#include <RF22.h>
#include <RF22Router.h>

#define MY_ADDRESS 5
#define NODE_ADDRESS_1 12
#define NODE_ADDRESS_2 13
#define NODE_ADDRESS_3 14
#define MAX_SLOTS 9

RF22Router rf22(MY_ADDRESS);

int parkingSlots[MAX_SLOTS];
int temperatures[MAX_SLOTS];

void setup() {
  Serial.begin(9600);
  if (!rf22.init())
    Serial.println("RF22 init failed");
  // Defaults after init are 434.0MHz, 0.05MHz AFC pull-in, modulation FSK_Rb2_4Fd36
  if (!rf22.setFrequency(431.0))
    Serial.println("setFrequency Fail");
  rf22.setTxPower(RF22_TXPOW_20DBM);
  //1,2,5,8,11,14,17,20 DBM
  rf22.setModemConfig(RF22::GFSK_Rb125Fd125);
  //modulation

  // Manually define the routes for this network
  rf22.addRouteTo(NODE_ADDRESS_1, NODE_ADDRESS_1);
  rf22.addRouteTo(NODE_ADDRESS_2, NODE_ADDRESS_2);
  rf22.addRouteTo(NODE_ADDRESS_3, NODE_ADDRESS_3);

}

void loop() {
  uint8_t buf[RF22_ROUTER_MAX_MESSAGE_LEN];
  char incoming[RF22_ROUTER_MAX_MESSAGE_LEN];
  memset(buf, '\0', RF22_ROUTER_MAX_MESSAGE_LEN);
  memset(incoming, '\0', RF22_ROUTER_MAX_MESSAGE_LEN);
  uint8_t len = sizeof(buf);
  uint8_t from;
  if (rf22.recvfromAck(buf, &len, &from))
  {
    buf[RF22_ROUTER_MAX_MESSAGE_LEN - 1] = '\0';
    memcpy(incoming, buf, RF22_ROUTER_MAX_MESSAGE_LEN);
    Serial.print("got request from : ");
    Serial.println(from, DEC);


  String receivedString = String((char*)incoming);

  // Split the received string into individual integers
  int car_parked = receivedString.substring(0, receivedString.indexOf(",")).toInt();
  receivedString.remove(0, receivedString.indexOf(",") + 1);
  int temperature = receivedString.substring(0, receivedString.indexOf(",")).toInt();

  parkingSlots[from] = car_parked;
  temperatures[from] = temperature;
  
  printIntArray(parkingSlots, MAX_SLOTS);
  printIntArray(temperatures, MAX_SLOTS);

  }

}

void printIntArray(int array[], int length) {
  for (int i = 0; i < length; i++) {
    Serial.print(array[i]);
    Serial.print(" ");
  }
  Serial.println();
}
