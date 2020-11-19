*This repository contains the firmware for the open source Roxie (based on [DAVEga OS](https://github.com/janpom/davega)).*
**For DAVEGA X (not open source), please go to [davega.eu](https://davega.eu).**

-----

Roxie displays speed, battery capacity, traveled distance and other useful info retrieved from a [VESC](https://www.vesc-project.com/) based controller.

![Roxie](img/roxie.jpg)

This repository contains the Roxie firmware. The gerber files can be downloaded from [roxie-hardware repository](https://github.com/charclo/roxie-hardware)

## Main features

- speedometer
- odometer
  - current trip
  - total traveled distance
- VESC fault codes
- metric or imperial units
- Arduino compatible FW
- low HW cost (~€15)
- known to work well with the VESC FW v3.48, v3.40 and v5.1
  - previous versions not tested but should work as well unless too old
- FOCBOX Unity supported

## Configuring and installing firmware

The firmware has to be configured for a particular e-skate. At the very least, the information about the wheel size, gearing, and the battery pack has to be provided.

For installation see the [wiki](https://github.com/charclo/roxie-firmware/wiki)