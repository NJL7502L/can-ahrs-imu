#include <Arduino.h>
#include <../lib/FlexCAN_T4/FlexCAN_T4.h>
#include <MsTimer2.h>

FlexCAN_T4<CAN3, RX_SIZE_256, TX_SIZE_16> CANbus;
CAN_message_t msg;

union FtoC{
  float f;
  uint8_t c[4];
};

FtoC pitch;
FtoC roll;
FtoC yaw;

void canbusRead(){
  CAN_message_t rxmsg;
  CANbus.read(rxmsg);
  if(rxmsg.id == 0x0A){
    pitch.c[0] = rxmsg.buf[0];
    pitch.c[1] = rxmsg.buf[1];
    pitch.c[2] = rxmsg.buf[2];
    pitch.c[3] = rxmsg.buf[3];

    roll.c[0] = rxmsg.buf[0];
    roll.c[1] = rxmsg.buf[1];
    roll.c[2] = rxmsg.buf[2];
    roll.c[3] = rxmsg.buf[3];
  }else if(rxmsg.id == 0x0B){
    yaw.c[0] = rxmsg.buf[0];
    yaw.c[1] = rxmsg.buf[1];
    yaw.c[2] = rxmsg.buf[2];
    yaw.c[3] = rxmsg.buf[3];
    digitalWrite(LED_BUILTIN,HIGH);
  }
}

// 一定周期毎に実行
void timerInt(){
  canbusRead();
}

void setup() {
  pinMode(LED_BUILTIN,OUTPUT);

  Serial.begin(115200);
  CANbus.begin();
  CANbus.setBaudRate(1000000);
  
  MsTimer2::set(1, timerInt);
  MsTimer2::start();
}

void printTitleValue(const char* str,float val){
  Serial.print(str);
  Serial.print(" :\t");
  Serial.print(val);
  Serial.print("\t");
}

void debugPrint(){
  printTitleValue("yaw",yaw.f);
  printTitleValue("pitch",pitch.f);
  printTitleValue("roll",roll.f);
  Serial.println();
}

void loop() {
  digitalWrite(LED_BUILTIN,LOW);
  delay(500);
  debugPrint();
}
