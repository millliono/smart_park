#include <RF22.h>
#include <RF22Router.h>

#define MY_ADDRESS 11 // define my unique address

// define who I can talk to
#define NODE_ADDRESS_1 1 
#define NODE_ADDRESS_2 2
#define NODE_ADDRESS_3 3
#define NODE_ADDRESS_4 4
#define NODE_ADDRESS_5 5
#define NODE_ADDRESS_6 6
#define NODE_ADDRESS_7 7
#define NODE_ADDRESS_8 8
#define NODE_ADDRESS_9 9
#define NODE_ADDRESS_10 10
#define NODE_ADDRESS_11 12
#define NODE_ADDRESS_12 13
#define NODE_ADDRESS_13 14
#define NODE_ADDRESS_14 15
#define NODE_ADDRESS_15 16
#define NODE_ADDRESS_16 17
#define NODE_ADDRESS_17 18
#define NODE_ADDRESS_18 19
#define NODE_ADDRESS_19 20

RF22Router rf22(MY_ADDRESS);


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
  rf22.addRouteTo(NODE_ADDRESS_4, NODE_ADDRESS_4);
  rf22.addRouteTo(NODE_ADDRESS_5, NODE_ADDRESS_5);
  rf22.addRouteTo(NODE_ADDRESS_6, NODE_ADDRESS_6);
  rf22.addRouteTo(NODE_ADDRESS_7, NODE_ADDRESS_7);
  rf22.addRouteTo(NODE_ADDRESS_8, NODE_ADDRESS_8);
  rf22.addRouteTo(NODE_ADDRESS_9, NODE_ADDRESS_9);
  rf22.addRouteTo(NODE_ADDRESS_10, NODE_ADDRESS_10);
  rf22.addRouteTo(NODE_ADDRESS_11, NODE_ADDRESS_11);
  rf22.addRouteTo(NODE_ADDRESS_12, NODE_ADDRESS_12);
  rf22.addRouteTo(NODE_ADDRESS_13, NODE_ADDRESS_13);
  rf22.addRouteTo(NODE_ADDRESS_14, NODE_ADDRESS_14);
  rf22.addRouteTo(NODE_ADDRESS_15, NODE_ADDRESS_15);
  rf22.addRouteTo(NODE_ADDRESS_16, NODE_ADDRESS_16);
  rf22.addRouteTo(NODE_ADDRESS_17, NODE_ADDRESS_17);
  rf22.addRouteTo(NODE_ADDRESS_18, NODE_ADDRESS_18);
  rf22.addRouteTo(NODE_ADDRESS_19, NODE_ADDRESS_19);
  rf22.addRouteTo(NODE_ADDRESS_20, NODE_ADDRESS_20);

}

void loop() {
  uint8_t buf[RF22_ROUTER_MAX_MESSAGE_LEN];
  char incoming[RF22_ROUTER_MAX_MESSAGE_LEN];
  memset(buf, '\0', RF22_ROUTER_MAX_MESSAGE_LEN);
  memset(incoming, '\0', RF22_ROUTER_MAX_MESSAGE_LEN);
  uint8_t len = sizeof(buf);
  uint8_t from;
  int received_value = 0;
  if (rf22.recvfromAck(buf, &len, &from))
  {
    buf[RF22_ROUTER_MAX_MESSAGE_LEN - 1] = '\0';
    memcpy(incoming, buf, RF22_ROUTER_MAX_MESSAGE_LEN);
    Serial.print("got request from : ");
    Serial.println(from, DEC);
    received_value = atoi((char*)incoming);
    Serial.println(received_value);
    //   delay(1000);
  }

}
