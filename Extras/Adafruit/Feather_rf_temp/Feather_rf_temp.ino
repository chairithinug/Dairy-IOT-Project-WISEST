#if defined(ARDUINO_SAMD_ZERO) && defined(SERIAL_PORT_USBVIRTUAL)
// Required for Serial on Zero based boards
#define Serial SERIAL_PORT_USBVIRTUAL
#endif

#define RST_POW 6 // Reset transistor to keep reading
#define RST_READER 5 // Reset reader always HIGH
#define COMPARE 128 // 1000_0000
#define BYTE_NUM_LENGTH 30 // Number of bytes receieved from sensor
#define LED 13

byte data_read, data_shift;
int shift_count = 0;
int index_byte = 0;

void setup() {
  pinMode(RST_POW, OUTPUT);
  pinMode(RST_READER, OUTPUT);
  pinMode(LED, OUTPUT);
  Serial.begin(9600);
  Serial1.begin(9600);

  while (!Serial) {
    delay(1);
  }
  digitalWrite(RST_POW, HIGH);
  delay(10);
  digitalWrite(RST_POW, LOW);
  delay(10);
  digitalWrite(RST_READER, HIGH);
  Serial.flush();
  Serial1.flush();
}

void loop() {
  if (Serial1.available() > 0) {
    data_read = Serial1.read();
    Serial.print(data_read);
    Serial.print("(");
    Serial.print(index_byte);
    Serial.print(") ");
    //padding(data_read);
    index_byte++;
  }
  if (index_byte >= BYTE_NUM_LENGTH) {
    digitalWrite(LED, HIGH);
    Serial.println("");
    Serial.flush();
    Serial1.end();
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

void padding(uint8_t data) { // adding leading zeros so the data is 8-bit wide
  shift_count = 0;
  data_shift = data;
  while (data_shift < COMPARE && shift_count < 8)
  {
    Serial.print(0, BIN);
    data_shift = data_shift << 1;
    shift_count = shift_count + 1;
    delay(1);
  }
  Serial.print(data, BIN);
}
