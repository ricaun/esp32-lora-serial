//----------------------------------------//
//  esp32-lora-serial.ino
//
//  created 17/06/2019
//  by Luiz Henrique Cassettari
//----------------------------------------//
//  update 28/06/2019
//  add server configuration name/lora
//----------------------------------------//

//----------------------------------------//
//  WIFI CONFIGURE
//----------------------------------------//
const char* ssid = "";
const char* password = "";

//----------------------------------------//
//  AP WIFI CONFIGURE
//----------------------------------------//
const char *APssid = "esp32";
const char *APpassword = "12345678";

//----------------------------------------//
//  LORA PINS
//----------------------------------------//
const int csPin = 18;
const int resetPin = 14;
const int irqPin = 26;

//----------------------------------------//
//  LORA CONFIG
//----------------------------------------//
long frequency = 919E6;
long signalBandwidth = 125E3;
int spreadingFactor = 7;

//----------------------------------------//
//  SEND CONFIG
//----------------------------------------//
int send_mode = 0;
String send_name = "BASE";

//----------------------------------------//
// setup
//----------------------------------------//
void setup() {
  Serial.begin(115200);
  while (!Serial);
  server_setup();
  load_config();
  oled_setup();
  lora_setup();
  button_setup();
}

void loop() 
{
  oled_loop();
  lora_loop();
  server_loop();
  if (button_loop())
  {
    oled_string(button_mode());
    if (button_count() == 0) 
    {
      server_off();
    }
    else
    {
      server_on();
    }
  }
}
