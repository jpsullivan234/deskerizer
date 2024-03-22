/*
 *  Created:  12/07/23 by John Sullivan
 *
 *  History:  Accidentally deleted all the old code. Redoing it here.
 *    12/14/23  - Made button checking functions non-obstructing.
 *              - Created a buffer system to allow for the mode-switching animation to be non - obstructive
 *    1/10/24   - Started implementing a 'settings' mode that is activated when up and down buttons are held simult.
 *              - issue: holding both buttons is difficult to detect bc it obstructs the on/off detection
 *    3/21/23   - Working on uploading this to gitHub
 *              - added solid rainbow
 *              - Need to fix: debounce issue where modes sometimes skip
 *              - Need to complete: settings mode
 * 
 *  Purpose:  Allows a NeoPixel strip to be controlled via 3 buttons between multiple modes and colors
 *  NOTE: To use, first change 'Hardware defs' settings to the pins and number of pixels you are using. Then, change settings as desired.
.*/

// LIBRARIES //
#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

// SETTINGS //
#define animate 1         // animate between modes or not (0 = off, 1 = on) (default 1)
#define animateDelay 3    // animation between modes delay, in ms (default 3)
#define debounce 20       // debounce time  (default 20)
#define longHold 1000     // mode button hold time  (default 1000ms or 1sec)
#define udHoldTime 300    // up and down button hold time (default 300 ms)
//#define fadeTime 10       // time between each color for 'fade' mode; dont define if settings mode enabled (default 10)
#ifndef fadeTime
  uint8_t fadeTime = 6;
#endif

// Hardware defs: 
#define mButton 2
#define uButton 3
#define dButton 4
#define LED_PIN 12
#define LED_COUNT 60
// Color defs
#define pureWhite 255,255,255
#define k5500 248,255,183
#define k4700 255,234,144
#define k3900 255,201,100
#define k3100 255,162,57
#define k2700 255,139,39
#define k1900 255,89,11
#define k1500 255,61,4
#define nothing 0,0,0
// Debugger
#define debugging 1
#if debugging == 1
  #define debug_init(x) Serial.begin(x)
  #define debug(x) Serial.print(x)
  #define debugln(x) Serial.println(x)
#else
  #define debug_init(x)
  #define debug(x)
  #define debugln(x)
#endif

// VARIABLES //
// Temporary
bool buttonLast = 0;
bool buttonVal = 0;
bool on = 1;
// Global
bool settingState = 0;
bool brightF = 0;
uint8_t brightness;
uint8_t mode;
uint8_t mButtonState;
uint8_t uButtonState;
uint8_t dButtonState;
// buffer vars
uint16_t hue = 0;
uint8_t bi = 0;
uint8_t x = 0;
uint8_t y = 255;
uint8_t bFlag = 0;
long startTime = -1;
uint32_t bColor;
// checkModeButton vars
bool mButtonVal = 1;
bool mButtonLast = 1;
bool mOK = 0;
bool mHoldEventPast = 0;
long upTime = -1;
long downTime = -1;
// checkButton vars
bool uButtonVal = 1;
bool uButtonLast = 1;
bool uOK = 0;
bool uHoldEventPast = 0;
bool uCancelHold = 0;
long uUpTime = -1;
long uDownTime = -1;
bool dButtonVal = 1;
bool dButtonLast = 1;
bool dOK = 0;
bool dHoldEventPast = 0;
bool dCancelHold = 0;
long dUpTime = -1;
long dDownTime = -1;

// FUNCTION DECLARATIONS //
void solid( uint8_t r, uint8_t g, uint8_t b, bool style);
void switchMode(bool style);
void buffer();
uint8_t checkModeButton(void);
uint8_t checkButton(uint8_t pin);

// OBJECTS //
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

// SETUP //
void setup(void) {
  debug_init(250000);
  debugln("start");

  pinMode(mButton, INPUT_PULLUP);
  pinMode(uButton, INPUT_PULLUP);
  pinMode(dButton, INPUT_PULLUP);

  strip.begin();                   
  strip.setBrightness(255); 

  brightness = 255;
  mode = 5;
  switchMode(animate); // start in mode 0
}

// LOOP //
void loop(void) {
  // Read each button
  mButtonState = checkModeButton();
  uButtonState = checkButton(uButton);
  dButtonState = checkButton(dButton);

  // Buffer the switching animation, if needed
  buffer();

  // Detect special combos
  if (mButtonState == 2 && uButtonState == 2){
    // allow change settings
    if (settingState) settingState = 0;
    else settingState = 1;
  }

  // setting mode (if on, loop won't go past here)
  if (settingState){
    switch(mode){
      case 2:
        debug("settings mode: fadeTime = ");
        debugln(fadeTime);
        if (uButtonState == 1) fadeTime++;
        if (dButtonState == 1) fadeTime--;
        if (fadeTime == 0) fadeTime = 1;
        if (fadeTime == 255) fadeTime = 254;
      return;
    }
  }

  // Decide what to do with mButton press
  switch (mButtonState){
    case 1: switchMode(animate); break;
    case 2:
      if (on){
        debugln("Off");
        solid(nothing, animate);
        on = 0;
      }else{
        debugln("On");
        mode -= 1;
        switchMode(animate);
        on = 1;
      }
    break;
  }
  
  // if off, loop won't go past here
  if (!on) return;

  // Decide what to do with up and down button presses
  switch(uButtonState){
    case 1:
      if (brightness < 240){
        brightness += 15;
        brightF = 1;
      }
    break;
    case 2:
      if (brightness < 255){
        brightness += 1;

        debugln(brightness);

        brightF = 1;  // increase brightness and set brightness flag
      }
    break;
  }
  switch(dButtonState){
    case 1:
      if (brightness > 16){
        brightness -= 15;
        brightF = 1;
      }
    break;
    case 2:
      if (brightness > 1){
        brightness -= 1;
        brightF = 1;  // increase brightness and set brightness flag
      }
    break;
  }

  // Once the brightness reaches the ends (255 or 0)
  if ((uButtonState > 0 && brightness == 255) || (dButtonState > 0 && brightness == 1)){ // blink once and disable hold
    uCancelHold = 1;
    dCancelHold = 1;
  }
  // Change the actual brightness on the leds
  if (brightF && bFlag < 2){
    strip.setBrightness(brightness); 
    mode--;
    switchMode(0);
    brightF = 0; // set brightness and clear flag
  } else if (brightF){
    strip.setBrightness(brightness);
    strip.show();
    brightF = 0;
  }

}

/* sets the entire strip to the specified color. Input: (uint8_t r, g, b) */
void solid( uint8_t r, uint8_t g, uint8_t b, bool style){
  uint32_t color = strip.Color(r,g,b);
  if (style){
    bFlag = 1;
    bColor = color;
    bi = 0;
  }
  else {
      strip.fill(color);
      strip.show();
  }  
}

/* Switch to the next mode. Input: style(1 = animate, 0 = dont) */
void switchMode(bool style){
  if (mode++ == 5) mode = 0; // 5 modes; wraps around at 5 inclusive
  debug("mode: "); debugln(mode);

  switch (mode){
    case 0: solid( k2700,     style); break;    // mode 0: solid warm white
    case 1: solid( k4700,     style); break;    // mode 1: solid cool white
    case 2: x = 0; y = 255; bFlag = 2; break;   // mode 2: fade
    case 3: solid( 255,0,0,   style); break;    // mode 3: solid red
    case 4: solid( 0,255,0,   style); break;    // mode 4: solid green
    case 5: solid( 0,0,255,   style); break;    // mode 5: solid blue
  }
}

/* Allows animations to occur while the loop is running so that buttons can still be pressed*/
void buffer(void){
  switch(bFlag){
    case 1: // mode switch animation
      if ((millis() - startTime) > animateDelay){   // every time 3ms has passed, change the next pixel
        
        strip.setPixelColor(bi++, bColor);
        strip.show();
        startTime = millis();   // reset startTime to allow another 3ms to be detected
      }
      if (bi == (LED_COUNT + 1)){  // Once all 60 leds have been turned on, reset i and clear bFlag
        bi = 0;
        bFlag = 0;
      }
    break;

    case 2: // fade using HSV
      if ((millis() - startTime > fadeTime) && hue < 771){  // actual hue ranges 0-65535, prescaler 85
        //debug(hue); debugln(", 255, 255");
        strip.fill(strip.ColorHSV((hue++ * 85),255,255));
        strip.show();
        startTime = millis();
      } else if (hue == 771) hue = 0;
    break;
    /*
    case 2: // fade
      debugln("2");

      if ((millis() - startTime) > fadeTime && x < 255){
        debug(x); debug(", "); debug(y); debugln(", 255");
        strip.fill(strip.Color(y--, x++, 255));
        strip.show();
        startTime = millis();
      } else if (x == 255) bFlag = 3;
    break;
    case 3:
      debugln("3");
      if ((millis() - startTime) > fadeTime && x > 0){
        debug("255, "); debug(x); debug(", "); debugln(y);
        strip.fill(strip.Color(y++,255,x--));
        strip.show();
        startTime = millis();
      } else if (x == 0) bFlag = 4;
    break;
    case 4:
      debugln("4");
      if ((millis() - startTime) > fadeTime && y > 0){
        debug(y); debug(", "); debugln(", 255"); debug(x); 
        strip.fill(strip.Color(255,y--,x++));
        strip.show();
        startTime = millis();
      } else if (y == 0) {
        bFlag = 2; 
        x = 0; 
        y = 255;
      }
    break;
    */
  }
}

/* Returns the state of the mode button (0=no press, 1= single press, 2= long hold)*/
uint8_t checkModeButton(void){
  // returns 0 if no press, 1 if single click, 2 if long hold
  // Only used for top button

  uint8_t event = 0;
  mOK = 0;
  mButtonVal = digitalRead(mButton);  // read button 

  // if button pressed
  if (mButtonVal == 0 && mButtonLast == 1 && (millis() - upTime) > debounce){
    downTime = millis();
    mHoldEventPast = 0;
  }

  // if button released
  else if (mButtonVal == 1 && mButtonLast == 0 && (millis() - downTime) > debounce && mHoldEventPast == 0){
    upTime = millis();
    mOK = 1;
  }

  // test for single click
  if (mButtonVal == 1 && mOK == 1) event =  1;

  // test for hold
  if (mButtonVal == 0 && (millis() - downTime) > longHold && mHoldEventPast == 0){
    event = 2;
    mHoldEventPast = 1;
  }

  mButtonLast = mButtonVal;
  return event;
}

/* Returns the state of either the up or down button input (0 = no press, 1= single, 2 = hold): pin of the button to check.*/
uint8_t checkButton(uint8_t pin){
  uint8_t event = 0;
  uOK = 0;
  dOK = 0;

  // Decide which button to check
  switch (pin){
    case uButton:
      uButtonVal = digitalRead(uButton);  // read button 

      // if button pressed
      if (uButtonVal == 0 && uButtonLast == 1 && (millis() - uUpTime) > debounce){
        uDownTime = millis();
        uHoldEventPast = 0;
        uCancelHold = 0;
      }

      // if button released
      else if (uButtonVal == 1 && uButtonLast == 0 && (millis() - uDownTime) > debounce && uHoldEventPast == 0){
        uUpTime = millis();
        uOK = 1;
      }

      // test for single click
      if (uButtonVal == 1 && uOK == 1) event =  1;

      // test for hold
      if (uButtonVal == 0 && (millis() - uDownTime) > udHoldTime && uCancelHold == 0){
        event = 2;
        uHoldEventPast = 1;
      }
      uButtonLast = uButtonVal;
    break;

    case dButton:
      dButtonVal = digitalRead(dButton);  // read button 

      // if button pressed
      if (dButtonVal == 0 && dButtonLast == 1 && (millis() - dUpTime) > debounce){
        dDownTime = millis();
        dHoldEventPast = 0;
        dCancelHold = 0;
      }

      // if button released
      else if (dButtonVal == 1 && dButtonLast == 0 && (millis() - dDownTime) > debounce && dHoldEventPast == 0){
        dUpTime = millis();
        dOK = 1;
      }

      // test for single click
      if (dButtonVal == 1 && dOK == 1) event =  1;

      // test for hold
      if (dButtonVal == 0 && (millis() - dDownTime) > udHoldTime && dCancelHold == 0){
        event = 2;
        dHoldEventPast = 1;
      }

      dButtonLast = dButtonVal;
    break;
  }
  return event;
}





