# deskerizer
NeoPixel Altoid tin multi-mode desk lamp using Arduino.

![IMG_4941](https://github.com/jpsullivan234/deskerizer/assets/92565104/0e99e306-a022-400a-996e-3e1bd6fcf210)

This is a project I made back in September 2023. **Here's the gist:**
  - Controlled by an Arduino Pro Mini (3v 8MHz)
  - Powered by a standard USB type C port
  - Everything is contained inside an Altoids can
  - 3 buttons: On/Off/Mode button, brightness up, brightness down
  - Programmed with 6 modes but you can add as many as you'd like using my code
  - The modes include solid colors, fade, and multi-color
  - Every function in the code is non-ubstructing, meaning that you can press a button and cancel the fade at any point,     for example

Feel free to clone this repository and try my code. The code was implemented using PlatformIO, but with some minor modifications it should work in the Arduino IDE as well. There are some small bugs that I haven't fixed yet, like the modes occasionally skip when you press the mode button because of a debounce flaw.

**Circuit:**
View the circuit schematic [here](https://github.com/jpsullivan234/deskerizer/blob/75a3da9ff55c3819afbcfb51c352b4ee03d1c138/DeskerizerSchematic.jpg)

**Code:**
To run the code, you'll need PlatformIo installed. I use the VScode extension. Change the hardware settings to match your design as shown below [(from this file).](https://github.com/jpsullivan234/deskerizer/blob/f3788d3a5447a76e29e0b0093865ea8c205f2d7d/src/main.cpp)

    // Hardware defs: 
    #define mButton 2
    #define uButton 3
    #define dButton 4
    #define LED_PIN 12
    #define LED_COUNT 60

Hit upload and if you did it right it'll work. 

**More Pictures:**

<img src="https://github.com/jpsullivan234/deskerizer/assets/92565104/016c1317-d104-4d2d-8d8b-1cd88fb30119" alt="IMG_4947" width="50%"/>

<img src="https://github.com/jpsullivan234/deskerizer/assets/92565104/088033b3-f2cd-4cc7-950f-b5e7b732eadd" alt="IMG_4945" width="50%"/>



