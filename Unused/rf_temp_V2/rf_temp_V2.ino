#define rst_n 8 //restart the reader to keep reading

//uint8_t compare = B10000000;
int data_read;

int index = 0;
void setup() {
  pinMode(rst_n, OUTPUT);
  Serial.begin(9600);
}

void loop() {
digitalWrite(rst_n,HIGH);
  
  if (Serial.available()>0) {
    data_read = Serial.read();
    Serial.println(data_read);
    index++;
  }
//    padding(data_read);
  
//  if (index == 30) {
//    Serial.flush();
//    Serial.println("");
//    index = 0;
//    delay(500);
//    digitalWrite(rst_n,LOW);
//    delay(500);
//  }
}

//void padding(uint8_t data){
//  data_shift = data;
//  while(data_shift < compare)
//  {
//    Serial.print(0,BIN);
//    data_shift = data_shift << 1;
//  }
//  Serial.print(data,BIN);
//}


