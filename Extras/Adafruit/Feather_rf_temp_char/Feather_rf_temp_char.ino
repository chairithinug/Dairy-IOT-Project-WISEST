#if defined(ARDUINO_SAMD_ZERO) && defined(SERIAL_PORT_USBVIRTUAL)
// Required for Serial on Zero based boards
#define Serial SERIAL_PORT_USBVIRTUAL
#endif

#define RST_POW 6 // Reset transistor to keep reading
#define RST_READER 5 // Reset reader always HIGH
#define DATA_NUM 30 // Number of bytes receieved from sensor
#define LED 13

const int PACKET_LENGTH = 3 * DATA_NUM;
byte data_read, data_shift;
int shift_count = 0;
int index_byte = 0;
char packet[PACKET_LENGTH] = {};

void setup() {
  pinMode(RST_POW, OUTPUT);
  pinMode(RST_READER, OUTPUT);
  pinMode(LED, OUTPUT);
  Serial1.begin(9600);

  digitalWrite(RST_POW, HIGH);
  delay(10);
  digitalWrite(RST_POW, LOW);
  delay(10);

  digitalWrite(RST_READER, HIGH);

  Serial1.flush();
  Serial.begin(9600);
}

void loop() {
  if (Serial1.available() > 0) {
    data_read = Serial1.read();
    itoa(data_read, packet + index_byte * 3, 10);
    index_byte = index_byte + 1;
  }
  if (index_byte >= DATA_NUM) {
    printPacket(packet, PACKET_LENGTH);
    
    digitalWrite(LED, HIGH);
    Serial1.end();
    memset(&packet[0], 0, sizeof(packet)); // clear memory
    index_byte = 0;
    digitalWrite(RST_POW, HIGH);
    delay(250);
    digitalWrite(RST_POW, LOW);
    Serial1.begin(9600);
    Serial1.flush();
    delay(250);
    digitalWrite(LED, LOW);
  }
}

void printPacket(char packet[], int packet_len) {
  for (int i = 0 ; i < packet_len; i++) {
    Serial.print(packet[i]);
  }
  Serial.println("");
}

