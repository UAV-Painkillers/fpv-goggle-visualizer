# CRSF Visualizer

## What is this?
This is an Open-Source project that aims to create a general purpose led controller (maybe other stuff as well in the future?) that reacts to channel values of an FPV Drone.
We achieve this by connecting a standard Receiver (needs to use CRSF protocol, e.g. Crossfire or ELRS Receivers) which gets bound in parallel to the "main" drone receiver so that both receivers receive the same signals from the Remote Controller.

## What do you need?
1. A transparent print of the Truerc Faceplate from [LetsFlyRC](https://www.youtube.com/@LetsFlyRC) (i used "[ABS Like Transparent Blue](https://www.3djake.com/elegoo/abs-like-resin-clear-blue?sai=13006)" Resin from Elegoo, but Transparent PETG for FDM printers should also work)
    - [https://www.thingiverse.com/thing:5967835](https://www.thingiverse.com/thing:5967835)
2. An ESP8266 or ESP32 micrcontroller
    - i used this one from aliexpress [https://aliexpress.com](https://de.aliexpress.com/item/1005004341489110.html?spm=a2g0o.order_list.order_list_main.16.13115c5f8k60hf&gatewayAdapt=glo2deu) pay attention! there is also a RP2040 and a ESP32 C3 (not S3) version, i did not test the RP2040 and the C3 definitely didnt work for me as there was a timing issue with the leds which made them flicker all the time
    - you can also easily use an ESP8266, the code supports both (i tested it with a Wemos D1 mini) [https://amazon.de](https://www.amazon.de/PNGOS-ESP8266-ESP-12F-Internet-Development/dp/B0BVFT8DY7/ref=sr_1_3?__mk_de_DE=%C3%85M%C3%85%C5%BD%C3%95%C3%91&crid=ES8R3011K0U&keywords=wemos+d1+mini&qid=1684917622&sprefix=wemos+d1+mini%2Caps%2C112&sr=8-3)
3. Some RGBIC leds (e.g. WS2812 or NEOPIXEL) that can run off 5V and fit inside the goggles. the "fit inside the goggles" is the biggest problem... in the end i bought some super small leds strips from aliexpress that fit PERFECTLY in the gap between the antenna housings and the faceplate walls
    - [https://aliexpress.com](https://de.aliexpress.com/item/4000744555910.html?spm=a2g0o.order_list.order_list_main.23.13115c5f8k60hf&gatewayAdapt=glo2deu)
4. obviously some wires

## Hardware Connections

### How to power the setup?
Great question, stupid simple solution.
We grab the power from the internal fan of the Goggles it delivers enough current for the leds and the fan itself. (You do this on your own Risk, i am not responsible for any damage resulting from this or anything else mentioned on this page).

### Pins & Configuration
all pins used for this project can be configured in the file `include/const.h`
if your esp does not have the "default" pins, you can easily change them.

the same goes for a lot of configuration variables,
you can for example set your home wifi ssid and password if you want to use your home network for OTA updates instead of the esp creating a Hotspot, you can also re-map the used radio channels if e.g. your throttle is not on channel 3 you can configure it to use a different channel.
just take a look at that file to see what you can configure (there is quite a lot)

### RX
by default this project uses pin `36` as RX and pin `35` as TX pins.
connect your receivers `RX to 35` (tx)
and your receivers `TX to 36` (rx)
you also need to connect your receivers `GND` and `5V/VCC` pad to the corresponding pads on the ESP.

### LEDS
by default this project uses pin `16` to control the RGBIC leds (`16 leds` by default).
Connect the `Data IN` pin of your leds to pin `16`,
you also need to connect the `GND` pin of the leds to `GND` of the ESP.
If you want to power your leds externally you need to connect + and - of the leds to your power supply,
otherwise you need to connect + to 5V and - to GND of the Wemos D1.

by default this project assumes you use `16 leds`.
if you want to control a different amount of leds, change the `NUM_LEDS` variable in the `include/const.h` file accordingly.

## RX Setup
if you managed to get this working with an RX (type) that is not listed here, feel free to add instructions in a PR/issue :) 

### ELRS

#### Flashing
As we use `SoftwareSerial` to talk to the RX, we need to limit the `Baud Rate to a maximum of 57600`.
Therefore we need to set this baud rate when flashing the elrs rx.
Apart from this, we dont need to set anything special on the rx configuration when flashing it.

Here is a Screenshot of a working configuration.
![Screenshot of ELRS Configurator](./images/rx-setup/elrs/configurator-config.jpg)

#### Disable Telemetry
In order to use multiple ELRS rx at the same time, we need to `disable the telemetry` for all but the main rx.
To do this, enable the `WiFi mode` of the rx (e.g. by waiting until it goes into wifi mode when no remote is powered on) and go to the Web-Interface of your rx.

usually you can access it via [http://elrs_rx.local/](http://elrs_rx.local/)

from the web interface, navigate to the `Model` tab and enable `Force telemetry OFF on this receiver`,
then hit `Save`

after this you should see a `success` message.

Your done, you can now connect your Main rx (e.g. Drone) and your crsf-visualizer rx at the same time 🎉

## OTA Updates
nobody wants to open up their goggles front plate or wherever you will put this visualizer only to install an update.
therefore this project uses `OTA` updates, which means you can connect the ESP to your wifi or open a hotspot connection through which you can then flash the latest firmware.
If you want the visualizer to connect to your local WiFi, please specify your home SSID and password in `include/const.h` before flashing.

To enable OTA update mode you need to turn on the visualizer and the remote.
Now you need to turn OFF the arm switch, turn ON the led controll switch and move both sticks to the top outside.
The visualizer should start blinking blue or purple.

- fast blue flashing = trying to connect to your wifi
- slow blue flashign = connected to your wifi
- slow purple flashing = hotspot mode active
    - Default SSID: `CRSF Visualizer`
    - Default Password: `jappy is awesome`

When connected to the hotspot, the esp's ip is `10.0.0.1`
When connected to your local wifi, the ip cannot be known in advance, therefore you need to check your router to figure out whats the ip of the esp.

If you are not using the hotspot, you need to change the ip of the esp in the `platformio.ini` file.

If you know want to flash the esp, just select the correct environment (all OTA variants end with `_ota`) in platformio and hit flash. (Of course you need to be connected to the same wifi/hotspot as the esp)

## Responsibility and Warranty

    Obviously this mod voids your warranty on the goggles.
    I am NOT responsible for any damage caused by this modification or the code provided.
    Anything you do is your own responsibility.
    If you dont have enough skills to do this yourself, please dont try it in the first place, these goggles are fucking expensive.