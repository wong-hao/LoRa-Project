
/*#include <SPI.h>
#include <LoRa.h>

int counter = 0;

void setup() {
  Serial.begin(115200);
  while (!Serial);

  Serial.println("LoRa Sender");
  LoRa.setSPI(SPI);
  LoRa.setSPIFrequency(10E6);
  if (!LoRa.begin(433E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }else
  {
    LoRa.setSpreadingFactor(8);
    LoRa.setSignalBandwidth(250E3);
    LoRa.setCodingRate4(7);
    LoRa.setPreambleLength(8);
    //LoRa.setSyncWord(0x12);
    LoRa.disableCrc();
  }
}

void loop() {
  Serial.print("Sending packet: ");
  Serial.println(counter);
  if(counter%2==0){
    LoRa.setTxPower(10);
  }else
  {
    LoRa.setTxPower(19);
  }

  // send packet
  LoRa.beginPacket(false);
  LoRa.print("test");
  //LoRa.print(counter);
  if(LoRa.endPacket(false)){
    Serial.println("Send success");
  }else{
    Serial.println("Send fail");
  }

  /*if(counter%2==1){
    delay(500);
  }
  counter++;
  delay(50);
}*/
#include <SPI.h>
#include <LoRa.h>

int counter = 0;

void setup() {
  Serial.begin(115200);
  while (!Serial);

  Serial.println("LoRa Sender");

  if (!LoRa.begin(433E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  LoRa.setSpreadingFactor(7);
  LoRa.setSignalBandwidth(125E3);
  LoRa.setCodingRate4(5);
  LoRa.disableCrc();
  LoRa.setPreambleLength(8);
  LoRa.setSyncWord(0x12);
}

void loop() {
  // send packet
  Serial.print("Sending packet: ");
  Serial.println(counter);
  //433 im 7 125 5 dis 
  if(counter%2==0){
    LoRa.setTxPower(2);
    LoRa.beginPacket(true);
    LoRa.print("hewei433 im 7 125 5 8 2 ");
    LoRa.endPacket();
  }else{
    LoRa.setTxPower(20);
    LoRa.beginPacket(true);
    LoRa.print("hewei433 im 7 125 5 8 20");
    LoRa.endPacket();
    delay(100);
  }
  counter++;
}
