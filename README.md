# PacoMouseCYD


Firmware for a WiFi throttle to control locomotives and accessories using a [Cheap Yellow Display](https://github.com/witnessmenow/ESP32-Cheap-Yellow-Display/)  2432S028R. 

![FirstVersion](images/PacoMouseCYD.png)

  * Protocols: Z21, Xpressnet LAN, Loconet over TCP/IP (LBserver & Binary) and ECoS
  * Control of Locomotives
  * Rotary encoder for loco speed and direction
  * Function icons
  * Color image of the locomotive from the SD.
  * Extra: Locomotive editor in the SD from the web browser
  * Loconet: Command station detection for the correct control of functions F9 to F28.
  * Shunting mode for precise stopping.
  * Steam locomotive driving simulator
  * Control of accesories with 2, 3 or 4 aspects.
  * Multiple panels of accessories.
  * CV and LNCV programming
  * Identify the name of the most common manufacturers when reading the CV8.
  * Locking of some features for guest or club use.
  * Manually measurement of train speed
  * Station Run: Game for children
  * WiFi Analyzer
  * Menus in different languages:

  ![Lang](images/languages.png)
  
---




  ## Videos

  [![PacoMouseRun](https://img.youtube.com/vi/YSfBQpVUhg8/0.jpg)](https://www.youtube.com/watch?v=YSfBQpVUhg8)

  [![PacoMouseAutomation](https://img.youtube.com/vi/auRIvvbzx6Q/0.jpg)](https://www.youtube.com/watch?v=auRIvvbzx6Q)


---

## Documentation
- https://usuaris.tinet.cat/fmco/
- https://fmcopaco.github.io/
- Read the [manual](doc/PacoMouseCYD_manual_v0.9.pdf) in the doc directory
- Step-by-step assembly of PacoMouseCYD by [Isaac](https://www.iguadix.es/content/pacomouse-cyd)

---

## Schematics

  ![Sch](images/schematics.png)

The CYD (Cheap Yellow Display) has the following features:

* ESP32 (With Wifi and Bluetooth)
* 320 x 240 TFT Display (2.8" ILI9341)
* Touch Screen (Resistive XPT2046)
* USB for powering and programming
* SD Card Slot (max. 32Gb FAT32), LED and some additional pins broken out in JST 1.25 connectors.


Just add a rotary encoder type EC-11 or KY-040 with a pushbutton, a battery and its charger to have your **PacoMouseCYD** wireless throttle.

---

## Copyright
Copyright (c) 2025-2026 Paco Cañada, [The Pows](https://usuaris.tinet.cat/fmco/)   
All rights reserved.

---

## License
Proprietary.  
Sources are only provided to compile and upload to the device.    
Modifiyng source code or forking/publishing this project ist not allowed.  
Commercial use is forbidden.  

---

## Used Libraries
  * TFT_eSPI (FreeBSD)
  


