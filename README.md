# CRSF Visualizer

## What is this?
This is an Open-Source project that aims to create a general purpose led controller (maybe other stuff as well in the future?) that reacts to channel values of an FPV Drone.
We achieve this by connecting a standard Receiver (needs to use CRSF protocol, e.g. Crossfire or ELRS Receivers) which gets bound in parallel to the "main" drone receiver so that both receivers receive the same signals from the Remote Controller.

## RX Setup
if you managed to get this working with an RX (type) that is not listed here, feel free to add instruction in a PR/issue :) 

### ELRS

#### Flashing
As we use SoftwareSerial to talk to the RX, we need to limit the Baud Rate to a maximum of 57600.
Therefore we need to set this baud rate when flashing the elrs rx.
Apart from this, we dont need to set anything special on the rx configuration when flashing it.

Here is a Screenshot of a working configuration.
![Screenshot of ELRS Configurator](./images/rx-setup/elrs/configurator-config.jpg)

#### Disable Telemetry
In order to use multiple ELRS rx at the same time, we need to disable the telemetry for all but the main rx.
To do this, enable the WiFi mode of the rx (e.g. by waiting until it goes into wifi mode when no remote is powered on) and go to the Web-Interface of your rx.

usually you can access it via [http://elrs_rx.local/](http://elrs_rx.local/)

from the web interface, navigate to the "Model" tab and enable "Force telemetry OFF on this receiver",
then hit "Save"

after this you should see a success message.

Your done, you can now connect your Main rx (e.g. Drone) and your crsf-visualizer rx at the same time 🎉

## Hardware Connections

### RX
by default this project uses pin D7 as RX and pin D8 as TX pins.
connect your receivers RX to D8 (tx)
and your receivers TX to D7 (rx)
you also need to connect your receivers GND and 5V/VCC pad to the corresponding pads on the Wemos D1.

### LEDS
by default this project uses pin D4 to control the RGBIC leds.
Connect the Data IN pin of your leds to pin D4,
you also need to connect the GND pin of the leds to GND of the Wemos D1.
If you want to power your leds externally you need to connect + and - of the leds to your power supply,
otherwise you need to connect + to 5V and - to GND of the Wemos D1.

by default this project assumes you use 9 leds.
if you want to control a different amount of leds, change the "NUM_LEDS" variable in the `include/const.h` file accordingly.

## OTA Updates
nobody wants to open up their goggles front plate or wherever you will put this visualizer only to install an update.
therefore this project uses "OTA" updates, which mean you can connect the Wemos D1 to your wifi or open a hotspot connection through which you can then flash the latest firmware.
If you want the visualizer to connect to your local WiFi, please specify your home SSID and password in `include/const.h` before flashing.

To enable OTA update mode you need to turn on the visualizer and the remote.
Now you need to turn OFF the arm switch, turn ON the led controll switch and move both sticks to the top outside.
The visualizer should start blinking blue.
When it starts breathing blue it is connected to your local wifi.
If it start breathing in purple it is in hotspot mode and you should see a new wifi called `CRSF Visualizer`, the standard password is `jappy is awesome`

When connected to the hotspot, the esp's ip is `10.0.0.1`
When connected to your local wifi, the ip cannot be known in advance, therefore you need to check your router to figure out whats the ip of the esp.

If you are not using the hotspot, you need to change the ip of the esp in the `platformio.ini` file.

If you know want to flash the esp, just select the `d1_mini_ota` environment in platformio and hit flash. (Of course you need to be connected to the same wifi/hotspot as the esp)