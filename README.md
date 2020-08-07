*This repository contains the firmware and gerber files for the open source Roxie (based on DAVEga OS). **For DAVEGA X (not open source), please go to [davega.eu](https://davega.eu).***

-----

Roxie displays speed, battery capacity, traveled distance and other useful info retrieved from a [VESC](https://www.vesc-project.com/) based controller.

![Roxie](img/roxie.png)

This repository contains the Roxie firmware. The gerber files can be downloaded from the [gerbers](gerbers/) directory.

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
- known to work well with the VESC FW v3.48 and v3.40
  - previous versions not tested but should work as well unless too old
- FOCBOX Unity supported

## Configuring and installing firmware

The firmware has to be configured for a particular e-skate. At the very least, the information about the wheel size, gearing, and the battery pack has to be provided.

1. Open the [Arduino IDE](https://www.arduino.cc/en/Main/Software).
2. Install the TFT_22_ILI9225 library.
   - Open the Arduino IDE and do `Tools > Manage Libraries... `, in this dialog search for `TFT_22_ILI9225` and install it.
3. Download Roxie firmware from [releases]() and unzip to your Arduino project directory (typically `$HOME/Documents/Arduino/`).
3. Open the [roxie.ino](roxie.ino) from Arduino IDE to load the project.
4. Go to the [roxie_config.h](roxie_config.h) tab and adjust the configuration to your needs.
5. Connect Roxie to your computer using a USB-UART adapter. Make sure you have drivers for the adapter installed. The DIY kit comes with the [CP2102](https://www.silabs.com/products/development-tools/software/usb-to-uart-bridge-vcp-drivers) based adapter.
6. Under Tools, select:
   - Board: Arduino Nano
   - Processor: ATmega328P
   - Port: the port to which the USB-UART is connected (e.g. `/dev/cu.SLAB_USBtoUART` for CP2102 adapter on a Mac)
7. Upload.

## Configuring the ESC

In the [VESC Tool](https://vesc-project.com/vesc_tool) set:

1. App Settings > General > App to Use: **PPM and UART**
2. App Settings > UART > Baudrate: **115200 bps**

## License

Roxie firmware and hardware design is released under [GNU GPL v3](LICENSE).

## Enclosures

3D-printable Roxie enclosures are available on the [Thingiverse](https://thingiverse.com/).
- [by @mmaner (v2)](https://www.thingiverse.com/thing:3218890)
- [by @mmaner (v3)](https://www.thingiverse.com/thing:3274207)
- [by @lrdesigns](https://www.thingiverse.com/thing:3171000)
- [by @JonathanLau1983](https://www.thingiverse.com/thing:3248802)

## Links

- Roxie 2.0" display purchase links
  - [AliExpress](https://www.aliexpress.com/item/ILI9225-2-0-Inch-UART-TFT-LCD-Display-Module-SPI-Interface-Colorful-Screen-Serial-Port-176x220/32792711665.html) (verified seller)
  - [Amazon](https://www.amazon.com/Display-176x220-ILI9225-Peripheral-Interface/dp/B07HF7WVST) (faster delivery but more expensive)
- [VESC project](https://www.vesc-project.com/)
- [DAVEGA X](https://davega.eu/) (more advanced commercial version of DAVEGA) 
