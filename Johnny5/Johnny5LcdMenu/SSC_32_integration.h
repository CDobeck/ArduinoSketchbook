
#define NUM_SERVOS 17

//Users Guide for SSC-32
//http://www.lynxmotion.com/images/html/build136.htm

//Binary Command Guide for SSC-32
//NOTE: Binary as in 0x80, may not be useful or necessary for arduino
//but it would be more efficient to transfer as binary
//http://www.lynxmotion.com/images/html/build177.htm


//servo: which servo (0 - 31)
//pos: position (500 - 2500)
//time: time in ms
void moveServo(byte servo, int pos, int time) {
    Serial.print("#");
    Serial.print(servo);
    Serial.print(" P");
    Serial.print(pos);
    Serial.print(" T");
    Serial.println(time);
    delay(time); //force Arduino to wait for the move to complete
}

void moveServo(byte servo, int pos) {
    Serial.print("#");
    Serial.print(servo);
    Serial.print(" P");
    Serial.println(pos);
}

//NOTE, 0x00 and 0xFFFF turn off servo, but 0xFFFF has jitter
void moveServoBin(uint8_t servo, uint16_t pos) {
  Serial.write(0x80 + servo);
  Serial.write((pos >> 8) & 0xFF);
  Serial.write(pos & 0xFF);
  Serial.write(0xA1);
  Serial.write(0x00);
  Serial.write(0x00);
  Serial.write(0x0D);
}
