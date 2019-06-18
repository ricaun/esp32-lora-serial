//----------------------------------------//
//  ESP32_lora_serial.ino
//
//  created 17/06/2019
//  by Luiz Henrique Cassettari
//----------------------------------------//


//----------------------------------------//
//  LORA CONFIG
//----------------------------------------//
const long frequency = 919E6;
const long signalBandwidth = 125E3;
const int spreadingFactor = 7;

//----------------------------------------//
//  LORA PINS
//----------------------------------------//
const int csPin = 18;
const int resetPin = 14;
const int irqPin = 26;

//----------------------------------------//
//  SEND CONFIG
//----------------------------------------//
const boolean send_mode = true;
const String send_name = "BASE X";

//----------------------------------------//
// setup
//----------------------------------------//
void setup() {
  Serial.begin(115200);
  while (!Serial);
  oled_setup();
  lora_setup();
}

void loop() 
{
  lora_loop();
  oled_loop();
}
