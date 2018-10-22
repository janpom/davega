VeGa (Vesc Gauge, or Vesc Gadget) is a device for displaying data read from a [VESC](https://www.vesc-project.com/) based controller, such as speed, remaining battery capacity, and travelled distance. The device is also known (or, rather, incorrectly referred to) as [Dave](https://www.electric-skateboard.builders/t/vega-battery-monitor-odometer-speedometer/71509/8?u=janpom) by [some people](https://www.electric-skateboard.builders/t/vega-battery-monitor-odometer-speedometer/71509/130?u=janpom), which is pretty annoying, especially since it seems to [begin to catch](https://www.electric-skateboard.builders/t/vega-battery-monitor-odometer-speedometer/71509/145?u=janpom) and having to rebrand is no fun!

![VeGa prototype](img/vega_prototype.jpg | width=300)

This repository contains the VeGa firmware. The schematics and gerber files can be obtained from [EasyEDA](https://easyeda.com/honza.pomikalek/VESC-Gauge).

## Main features

- speedometer
- odometer
  - current trip
  - total traveled distance
- remaining battery capacity
  - battery voltage
  - battery amp-hours remaining
- Arduino compatible FW
- low HW cost (~$10)
- works with the latest VESC FW (v3.40)
  - previous versions not tested but should work as well unless too old

## Configuring and installing firmware

The firmware has to be configured for a particular e-skate. At the very least, the information about the wheel size, gearing, and the battery pack has to be provided.

1. Download the code and open it in the [Arduino IDE](https://www.arduino.cc/en/Main/Software). 
2. Open the [vega.ino](vega.ino) file and customize the options in the CONFIG section.
3. Connect VeGa to your computer using a USB-UART adapter. Make sure you have drivers for the adapter installed. The DIY kit comes with the [CP2102](https://www.silabs.com/products/development-tools/software/usb-to-uart-bridge-vcp-drivers) based adapter.
4. Under Tools, select:
   - Board: Arduino Nano
   - Processor: ATmega328P
   - Port: the port to which the USB-UART is connected (e.g. `/dev/cu.SLAB_USBtoUART` for CP2102 adapter on a Mac)
5. Upload.

## License

VeGa firmware and hardware design is released under [GNU GPL v3](LICENSE).

## Links

- [VeGa video presentation](https://youtu.be/u4e83JhVZNA)
- [VeGa thread at esk8 builders forum](https://www.electric-skateboard.builders/t/vega-battery-monitor-odometer-speedometer/71509)
- [VESC project](https://www.vesc-project.com/)
