#include <RF22.h>
#include <RF22Router.h>

#define MY_ADDRESS 5
#define NUM_PARK_SLOTS 9
#define OPEN_AC_TEMP 10
#define AC_LED 7
#define RESET 5

RF22Router rf22(MY_ADDRESS);

int parkingSlots[NUM_PARK_SLOTS];
int temperatures[NUM_PARK_SLOTS];
int num_sender[NUM_PARK_SLOTS];
int active[NUM_PARK_SLOTS];

float mean_temp = 0;
int empty_park_slots = 0;
int num_messages = 0;

void setup()
{
  Serial.begin(9600);
  if (!rf22.init())
    Serial.println("RF22 init failed");
  if (!rf22.setFrequency(431.0))
    Serial.println("setFrequency Fail");
  rf22.setTxPower(RF22_TXPOW_20DBM);
  rf22.setModemConfig(RF22::GFSK_Rb125Fd125);

  for (int i = 0; i < NUM_PARK_SLOTS; i++)
  {
    num_sender[i] = 0;
    active[i] = 0;
  }
  pinMode(AC_LED, OUTPUT);
}

void loop()
{
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

    String receivedString = String((char *)incoming);

    // Split the received string into individual integers
    int car_parked = receivedString.substring(0, receivedString.indexOf(",")).toInt();
    receivedString.remove(0, receivedString.indexOf(",") + 1);
    int temperature = receivedString.substring(0, receivedString.indexOf(",")).toInt();

    parkingSlots[from] = car_parked;
    temperatures[from] = temperature;
    num_sender[from]++;

    mean_temp = mean(temperatures, NUM_PARK_SLOTS);
    empty_park_slots = num_empty(parkingSlots, NUM_PARK_SLOTS);
    num_messages++;

    print_table(parkingSlots, temperatures);
    print_info(empty_park_slots, mean_temp);
    Serial.println("-------------------------");
  }
  open_ac(mean_temp);
  set_active();
  reset_active();

}

void print_arr(int array[], int length)
{
  for (int i = 0; i < length; i++)
  {
    Serial.print(array[i]);
    Serial.print(" ");
  }
  Serial.println();
}

double mean(int arr[], int size)
{
  int sum = 0;
  for (int i = 0; i < size; i++)
  {
    sum += arr[i];
  }
  int not_zero = 0;
  for (int i = 0; i < size; i++)
  {
    if (arr[i] != 0)
      not_zero++;
  }
  return (double)sum / not_zero;
}

int num_empty(int arr[], int size)
{
  int not_zero = 0;
  for (int i = 0; i < size; i++)
  {
    if (arr[i] == 1)
      not_zero++;
  }
  return (size - not_zero);
}

void open_ac(float mean_temperature)
{
  if (mean_temperature > OPEN_AC_TEMP)
  {
    digitalWrite(AC_LED, HIGH);
  }
  else
  {
    digitalWrite(AC_LED, LOW);
  }
}

void print_info(int empty_park_slots, int mean_temp)
{
  Serial.print("empty parking spaces: ");
  Serial.println(empty_park_slots);

  Serial.print("mean temperature: ");
  Serial.println(mean_temp);
}

void print_table(int park_slots[], int temperatures[])
{
  print_arr(park_slots, NUM_PARK_SLOTS);
  print_arr(temperatures, NUM_PARK_SLOTS);
  print_arr(num_sender, NUM_PARK_SLOTS);
  print_arr(active, NUM_PARK_SLOTS);
}

void set_active()
{
  for (int i = 0; i < NUM_PARK_SLOTS; i++)
  {
    if (num_sender[i] > 0)
    {
      active[i] = num_sender[i];
    }
    else
    {
      active[i] = 0;
    }
  }
}

void reset_active()
{
  if (num_messages >= RESET)
  {
    num_messages = 0;
    for (int i = 0; i < NUM_PARK_SLOTS; i++)
    {
      num_sender[i] = 0;
    }
  }
}
