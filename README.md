DAVEga (The VEsc GAuge/GAdget), or Dave for short, is a device for displaying data read from a [VESC](https://www.vesc-project.com/) based controller, such as speed, remaining battery capacity, and travelled distance. It was originally named VeGa, but was later re-branded due to the insistence of the e-skate lobby ([[1]](https://www.electric-skateboard.builders/t/davega-battery-monitor-odometer-speedometer/71509/8?u=janpom), [[2]](https://www.electric-skateboard.builders/t/davega-battery-monitor-odometer-speedometer/71509/145?u=janpom), [[3]](https://www.electric-skateboard.builders/t/davega-battery-monitor-odometer-speedometer/71509/213?u=janpom)).

![DAVEga](img/davega.jpg)

This repository contains the DAVEga firmware. The gerber files can be downloaded from the [gerbers](gerbers/) directory or from [EasyEDA](https://easyeda.com/honza.pomikalek/VESC-Gauge).

## Main features

- speedometer
- odometer
  - current trip
  - total traveled distance
- remaining battery capacity
  - battery voltage
  - battery amp-hours remaining
- VESC fault codes
- metric or imperial units
- Arduino compatible FW
- low HW cost (~$10)
- works with the latest VESC FW (v3.40)
  - previous versions not tested but should work as well unless too old

## Configuring and installing firmware

The firmware has to be configured for a particular e-skate. At the very least, the information about the wheel size, gearing, and the battery pack has to be provided.

1. Open the [Arduino IDE](https://www.arduino.cc/en/Main/Software).
2. Install the TFT_22_ILI9225 library.
   - Go to [https://github.com/Nkawu/TFT_22_ILI9225](https://github.com/Nkawu/TFT_22_ILI9225) and do `Clone or download > Download ZIP`.
   - Open the Arduino IDE and do `Sketch > Include Library > Add ZIP file...` Then select the downloaded ZIP file.  
3. Download DAVEga firmware from [releases](https://github.com/janpom/davega/releases) and unzip to your Arduino project directory (typically `$HOME/Documents/Arduino/`).
3. Open the [davega.ino](davega.ino) from Arduino IDE to load the project.
4. Go to the [davega_config.h](davega_config.h) tab and adjust the configuration to your needs.
5. Connect DAVEga to your computer using a USB-UART adapter. Make sure you have drivers for the adapter installed. The DIY kit comes with the [CP2102](https://www.silabs.com/products/development-tools/software/usb-to-uart-bridge-vcp-drivers) based adapter.
6. Under Tools, select:
   - Board: Arduino Nano
   - Processor: ATmega328P
   - Port: the port to which the USB-UART is connected (e.g. `/dev/cu.SLAB_USBtoUART` for CP2102 adapter on a Mac)
7. Upload.

## Configuring the ESC

In the [VESC Tool](https://vesc-project.com/vesc_tool) set:

1. App Settings > General > App to Use: PPM and UART
2. App Settings > UART > Baudrate: 115200 bps

## License

DAVEga firmware and hardware design is released under [GNU GPL v3](LICENSE).

## Links

- [DAVEga video presentation](https://youtu.be/u4e83JhVZNA)
- [DAVEga thread at esk8 builders forum](https://www.electric-skateboard.builders/t/davega-battery-monitor-odometer-speedometer/71509)
- [VESC project](https://www.vesc-project.com/)
