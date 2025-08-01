/* PIN Configuration */
#define SR05_EN_PIN       6
#define SR05_RX_PIN       2
#define LED_PIN           8   //ESP32-C3

/* Application */
#define SR05_DATA_LEN     4

/* Variables */
int led_state = HIGH;
int i = 0;
uint16_t sr05_data[SR05_DATA_LEN] = {0,0,0,0};
unsigned long read_timeout = 0;

/* Functions */
void setup() 
{
  Serial.begin(115200);
  Serial1.begin(9600, SERIAL_8N1, SR05_RX_PIN, -1);

  pinMode(SR05_EN_PIN, OUTPUT);
  digitalWrite(SR05_EN_PIN, HIGH);
  pinMode(LED_PIN, OUTPUT);
}
 
void MeasureDistanceBlocking()
{
  while( Serial1.read() >= 0 );

  digitalWrite(SR05_EN_PIN,0);
  read_timeout = millis();
  i = 0;
  do {
    while (Serial1.available() && i < SR05_DATA_LEN) {
      sr05_data[i++] = Serial1.read();
    }
  } while (i < SR05_DATA_LEN && millis() - read_timeout < 1000);
  digitalWrite(SR05_EN_PIN,1);

  /*
   * Data structure
   *  |    Header     |    High bytes (mm)   |    Low bytes (mm)   |   CRC (bytes 1 + bytes 2) & 0xFF |
   *        0xFF
   */
  if ( i >= SR05_DATA_LEN && sr05_data[0] == 0xff )
  {
    uint16_t distance = sr05_data[1] << 8 | sr05_data[2];
    if(sr05_data[3] == ((sr05_data[1] + sr05_data[2]) - 1)) {
      Serial.print("Distance : ");
      Serial.print(distance);
      Serial.println(" mm");
    } else {
      Serial.println("ERROR: CRC");
    }
  }
  else
  {
    if (i < SR05_DATA_LEN) {
      Serial.println("ERROR: Timeout");
    } else {
      Serial.print("Invalid data: 0x"); Serial.println(sr05_data[0], HEX);
    }
  }
}
 
void loop() 
{
  MeasureDistanceBlocking();

  digitalWrite(LED_PIN, led_state);
  led_state = ( led_state == HIGH )? LOW : HIGH;

  delay(1000);
}
