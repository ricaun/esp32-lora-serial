# ESP32_lora_serial

This sample code sends every byte from serial to LoRa, and every byte receives from the LoRa to the serial.

The code was designed to only send or receive, this means you need two boards, a sender and a receiver.

You can configure the sender and the receiver using the variable `send_mode`.
* `send_mode = true` only receive from serial and send to LoRa.
* `send_mode = false` only receive from LoRa and send to Serial.

The variable `send_name` is used to send a string to LoRa and the receiver shows on the OLED. 
These bytes are filtered by the code and not send to the serial. Every LoRa package who start with `##` gonna be filter and shows on the OLED.

The variables `csPin`, `resetPin` and `irqPin` is used to configure the pins from the LoRa module.

The variables `frequency`, `signalBandwidth` and `spreadingFactor` is used to configure the LoRa module frequency, BW and SF.

## Hardware 

* Heltec Wifi LoRa 32 (2x)

## Librarys

* [arduino-LoRa](https://github.com/sandeepmistry/arduino-LoRa)
* [ThingPulse OLED SSD1306](https://github.com/ThingPulse/esp8266-oled-ssd1306)
