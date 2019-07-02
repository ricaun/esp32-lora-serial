//----------------------------------------//
//  lora_serial.ino
//
//  created 17/06/2019
//  by Luiz Henrique Cassettari
//----------------------------------------//
//  update 28/06/2019
//  add millis to count last send / receive
//----------------------------------------//

#include <SPI.h>
#include <LoRa.h>

#define SEND_BUFFER_MAX 250
#define SERIAL_RX_BUFFER 2056
#define OLED_CHAR '#'
#define OLED_SEND_TIME 60000

static unsigned long loraMillis;

void lora_setup() {
  LoRa.setPins(csPin, resetPin, irqPin);

  if (!LoRa.begin(frequency)) {
    Serial.println("LoRa init failed. Check your connections.");
    oled_display("LoRa init failed.");
    while (true);
  }
  String oled = "LoRa init succeeded.\n";
  oled += frequency;
  oled_display(oled);
  Serial.println("LoRa init succeeded.");
  Serial.println();
  Serial.println();
  Serial.setRxBufferSize(SERIAL_RX_BUFFER);
  LoRa.setFrequency(frequency);
  LoRa.setSpreadingFactor(spreadingFactor);
  LoRa.setSignalBandwidth(signalBandwidth);
  delay(1000);
  if (send_mode){
    LoRa_txMode();
    //oled_status("TX MODE");
    //oled_string(send_name);
  }
  else
  {
    LoRa.onReceive(onReceive);
    LoRa_rxMode();
    //oled_status("RX MODE");
  }
}

void lora_loop() {
  if (send_mode){
    lora_send();
    lora_serial_read();
  }
}

void lora_serial_read() {
  if (Serial.available())
  {
    byte buffer[SEND_BUFFER_MAX];
    int i = 0;
    while (Serial.available()) {
      byte b = Serial.read();
      buffer[i++] = b;
      if (i == SEND_BUFFER_MAX) break;
    }
    String str = "";
    str += "Send: ";
    str += i;
    str += "\n";
    oled_status(str);
    LoRa.setFrequency(frequency);
    LoRa.setSpreadingFactor(spreadingFactor);
    LoRa.setSignalBandwidth(signalBandwidth);
    LoRa.beginPacket();
    LoRa.write(buffer, i);
    LoRa.endPacket();
    loraMillis = millis();
  }
}

void lora_send() {
  if (!Serial.available())
  {
    if (runEvery(OLED_SEND_TIME)) {
      String buffer = "";
      buffer += OLED_CHAR;
      buffer += OLED_CHAR;
      buffer += send_name;
      String str = "";
      str += "Send: ";
      str += buffer.length();
      str += "\n";
      oled_status(str);
      LoRa.setFrequency(frequency);
      LoRa.setSpreadingFactor(spreadingFactor);
      LoRa.setSignalBandwidth(signalBandwidth);
      LoRa.beginPacket();
      LoRa.print(buffer);
      LoRa.endPacket();
      loraMillis = millis();
    }
  }
}


void LoRa_rxMode() {
  LoRa.receive();
}

void LoRa_txMode() {
  LoRa.idle();
}

void onReceive(int size) {

  loraMillis = millis();
  
  String str = "";
  str += "Rssi: ";
  str += LoRa.packetRssi();
  str += "\n";
  str += "Receive: ";
  str += size;
  str += "\n";
  oled_status(str);
  char buffer[255];

  for(int i = 0; i < size; i++)
  {
    buffer[i] = LoRa.read();
  }
  
  if(buffer[0] == OLED_CHAR && buffer[1] == OLED_CHAR)
  {
    String oled = "";
    for (int i = 2; i< size; i++){
      oled += buffer[i];
    }
    oled_string(oled);
  }
  else
  {
    Serial.write((uint8_t*)buffer, size);
  }
}

boolean runEvery(unsigned long interval)
{
  static unsigned long previousMillis = 0;
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval)
  {
    previousMillis = currentMillis;
    return true;
  }
  return false;
}
