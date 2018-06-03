import processing.serial.*; 
 
Serial myPort;    // The serial port
String inString;  // Input string from serial port
int lf = 10;      // ASCII linefeed 
 
void setup() {
  myPort = new Serial(this, Serial.list()[0], 9600); 
  myPort.bufferUntil(lf); 
} 
 
void draw() { 
  if (myPort.available()>0)
    println("received: " + inString); 
} 
 
void serialEvent(Serial p) { 
  inString = p.readString(); 
} 
