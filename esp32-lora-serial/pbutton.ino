//----------------------------------------//
//  pbutton.ino
//
//  created 03/06/2019
//  by Luiz Henrique Cassettari
//----------------------------------------//
//  update 28/06/2019
//  add button time to turn off wifi
//----------------------------------------//

#define BUTTON 0
#define BUTTON_MODE_MAX 2

#define BUTTON_RUNEVERY 300000

static int button_i = 1;

void button_setup()
{
  pinMode(BUTTON, INPUT_PULLUP);
}

boolean button_press()
{
  static boolean last;
  boolean now = digitalRead(BUTTON);
  boolean ret = (now == false & last == true);
  last = now;
  return ret;
}

boolean button_loop()
{
  if (button_runEvery(BUTTON_RUNEVERY))
  {
    if (button_i != 0)
    {
      button_i = 0;
      return true;
    }
  }
  
  if (button_press())
  {
    button_runEvery(0);
    button_i++;
    return true;
  }
  return false;
}

int button_count()
{
  return button_i;
}

String button_mode()
{
  button_i = button_i % BUTTON_MODE_MAX;
  switch (button_i) {
    case 0:
      return "MODE WIFI OFF";
    case 1:
      return "MODE WIFI ON";
  }
}

boolean button_runEvery(unsigned long interval)
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
