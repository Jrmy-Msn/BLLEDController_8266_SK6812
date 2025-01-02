# FORK of [BL Led Controller](https://github.com/DutchDevelop/BLLEDController_8266))

>What's different ?
>manage a strip LED with 3 wires (Vcc,Gnd, data) using the [Adafruit_NEOPixel libraty](https://github.com/adafruit/Adafruit_NeoPixel.git)
>
>*Only 3 PINs needed onto ESP (Vcc 5v, GND, and 1 pin for data => default GPIO4)*

## BL Led Controller

The BL Led Controller is an ESP8266 / ESP32 based device that connects to your Bambulab X1,X1C,P1P Or P1S and controls the LED strip based on the state of the printer.

### Features

- Connects to Bambulab X1,X1C,P1P Or P1S
- Controls LED strip based on printer state

### Setup Instructions

Make sure rename `src/_config.h` to `src/config.h` (remove **_** char from name fo the file)
Then fill all printerConfig variables with your own.
example :
```c
char SSID[32] = "<wifi SSID>";
char APPW[32] = "<wifi password for SSID>";
char printerIP[16] = "<printer local IP>";
char accessCode[9] = "<printer access code>"; //find it in parameter -> General
char serialNumber[16] = "<printer serial number (SN:XXXXXXXXXXXXXX)>"; // find it in parameter -> General -> Device info
```

You can know compile and upload the code to your ESP8266.
For information, I use the WEMOS D1 MINI Lite.

### License

The BL Led Controller is released under Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International (CC BY-NC-SA 4.0) license. See the [LICENSE](https://github.com/DutchDevelop/BLLEDController/blob/main/LICENSE) file for more details.

### Credits
[DutchDeveloper]: Lead programmer
[Modbot]: Tester: X1C, P1P & P1S
[xps3riments]: Inspiration for foundation of the code

### Author

This project was created by [DutchDeveloper](https://dutchdevelop.com/).
