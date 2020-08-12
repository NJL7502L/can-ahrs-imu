  
#include <Arduino.h>
#include <../lib/FlexCAN_T4/FlexCAN_T4.h>
#include <MsTimer2.h>

const uint16_t RMD_ID = 0x140 + 1;
const uint8_t CONTROL_COMMAND[23] = {
    0x30,0x31,0x32,0x33,0x34,
    0x90,0x91,0x19,0x92,0x94,
    0x9A,0x9B,0x9C,0x9D,
    0x80,0x81,0x88,
    0xA1,0xA2,0xA3,0xA4,0xA5,0xA
};

FlexCAN_T4<CAN3, RX_SIZE_256, TX_SIZE_16> CANbus;
CAN_message_t msg;

uint16_t rawEncVal[2];

static uint32_t pos = 9000;
//              (モータのアドレス, データ長, 中身)
void canbusWrite(uint8_t addr,uint8_t len,uint8_t *message){
  msg.len = len;
  msg.id = RMD_ID + addr;
  for (int i = 0; i < len; i++) {
    msg.buf[i] = message[i];
  }
  CANbus.write(msg);
  digitalWrite(LED_BUILTIN,HIGH);
}

void canbusRead(){
  CAN_message_t rxmsg;
  CANbus.read(rxmsg);
  if(rxmsg.buf[0] == CONTROL_COMMAND[20]){
    rawEncVal[rxmsg.id - RMD_ID] = rxmsg.buf[2];
    rawEncVal[rxmsg.id - RMD_ID] += rxmsg.buf[3] << 8;
  }
}

// 一定周期毎に実行
void timerInt(){
  canbusRead();
  uint8_t buf[8] = {0};

  buf[0] = CONTROL_COMMAND[20];

  uint16_t speed = 720 * 4;
  buf[2] = speed;
  buf[3] = speed >> 8;
  buf[4] = pos;
  buf[5] = pos >> 8;
  buf[6] = pos >> 16;
  buf[7] = pos >> 24;

  canbusWrite(0,8,buf);
}

void setup() {
  pinMode(LED_BUILTIN,OUTPUT);

  Serial.begin(115200);
  CANbus.begin();
  CANbus.setBaudRate(1000000);
  
  MsTimer2::set(1, timerInt); //[ms]//0.5msごとでも良さそう。
  MsTimer2::start();
}

void loop() {

  digitalWrite(LED_BUILTIN,LOW);
  delay(500);
  pos += 18000; 
  Serial.println(rawEncVal[0]);
}