#  PacoMouseCYD 3D files

## Jeroen van der lee


* TPS63020 buck/boost with 1 battery and charger

When using the TPS63020 buck/boost 3.3V. you need to desolder both the voltage regulators on the ESP32 board and feed the "out" of the buck/boost converter to both the 3.3V "out" pads of those removed regulators.
To remove those, use flux and 2 soldering irons. then its easy done.


* TPS63020 buck/boost with 2 batteries

I decided to use the small TPS63020 buck/boost circuitboard and redesigned a case for two 2000 mAh 3.7V in parallel. The case is only a few millimeter thicker than the 1 battery version


i had to loose the charging but for me thats no problem. There is enough room inside to get it all sorted. a connector for the parallel batteries i made with dupont pinheader, works fine.




