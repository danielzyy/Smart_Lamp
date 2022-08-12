#include <TimeLib.h>
#include <Adafruit_CircuitPlayground.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "RTClib.h"

#define DEBUG true

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
#define TEMP_OFFSET -3.9
#define RING_SPEED_MS 15
#define RAINBOW_SPEED 10

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET, SCREEN_ADDRESS);
RTC_DS1307 rtc;

// Pin numbers
int encoderPinA = 0;
int encoderPinB = 6;
int lightButtonPin = 10; // encoder button
int modeButtonPin = 1;

enum Modes {
  MANUAL = 0, // Control lamp manually using encoder
  SOUND, // Control lamp using sound level
  ALARM, // Set alarm
  END
};
enum Modes mode = MANUAL;

// Button and light states
int currLightButtonState = 0;
int prevLightButtonState = 0;
int currModeButtonState = 0;
int prevModeButtonState = 0;
bool leftButtonState = false;
bool rightButtonState = false;
bool isRainbow = true;
int colorWheelState = 0;

// LED rainbow speed
int ledTimer = millis();
float peakSound = 0;
int brightness = 90;
// Sound light
bool isLightOn = false;
int soundLimit = 80;
int soundTimer = millis();

// Temperature
float avgTemp = 0;
float sumTemp = 0;
int numTemps = 0;
int tempTimer = millis();

// Alarm timer
int alarm_h = 0;
int alarm_m = 0;
bool isAlarmOn = false;

bool isLightButtonPressed() {
  if(currLightButtonState == 1 && prevLightButtonState == 0)
    return true;
  else
    return false;
}

bool isModeButtonPressed() {
  if(currModeButtonState == 1 && prevModeButtonState == 0)
    return true;
  else
    return false;
}

bool isLeftButtonPressed() {
  if(leftButtonState == false && CircuitPlayground.leftButton() == true)
    return true;
  else
    return false;
}

bool isRightButtonPressed() {
  if(rightButtonState == false && CircuitPlayground.rightButton() == true)
    return true;
  else
    return false;
}

void ringCycle(int ledSpeed) {
  if (millis()-ledTimer>ledSpeed && isRainbow == true) {
    colorWheelState++;
    ledTimer = millis();
  }
  if (colorWheelState > 255)
    colorWheelState = 0;
  for(int i=0; i<10; i++) {
    CircuitPlayground.setPixelColor(i,CircuitPlayground.colorWheel(colorWheelState));
  }
  if(isLightButtonPressed()) {
    isRainbow = !isRainbow;
    #if DEBUG
      Serial.println("TOGGLED LIGHT");
    #endif
  }
}

void rainbowCycle(int rainbowSpeed) {
  int offset = millis() / rainbowSpeed;
  // Loop through each pixel and set it to an incremental color wheel value.
  for(int i=0; i<10; ++i) {
    CircuitPlayground.strip.setPixelColor(i, CircuitPlayground.colorWheel(((i * 256 / 10) + offset) & 255));
  }
  CircuitPlayground.strip.show();
}

void manualLight() {
  if (CircuitPlayground.slideSwitch()) {
    ringCycle(RING_SPEED_MS);
  }
  else {
    rainbowCycle(RAINBOW_SPEED);
  }
}

void soundLight() {
  float soundLevel = CircuitPlayground.mic.soundPressureLevel(10);
  if (millis()-soundTimer < 100) {
    if(soundLevel > peakSound)
      peakSound = soundLevel;
  } else {
    if (peakSound > soundLimit) {
      isLightOn = !isLightOn;
      #if DEBUG
        Serial.print("Sound toggled at: ");
        Serial.println(peakSound);
      #endif
    }
    soundTimer = millis();
    peakSound = 0;
  }
  if (isLightOn) {
    if (CircuitPlayground.slideSwitch()) {
      ringCycle(RING_SPEED_MS);
    }
    else {
      rainbowCycle(RAINBOW_SPEED);
    }
  }
  else {
    CircuitPlayground.clearPixels();
  }
}

void updateTemp() {
  float temp = CircuitPlayground.temperature()+TEMP_OFFSET;
  if (millis()-tempTimer < 1000) {
    sumTemp += temp;
    numTemps++;
  } else {
    if(numTemps == 0) // when alarm is ringing, numTemps does not increment
      avgTemp = temp;
    else
      avgTemp = sumTemp/numTemps;
    sumTemp = 0;
    numTemps = 0;
    tempTimer = millis();
  }
}

void display_text() {
  DateTime now = rtc.now();
  // mode text
  display.clearDisplay();
  display.setCursor(0,0);
  display.setTextSize(1);
  display.print("Mode:");
  display.print(mode);
  // sound limit
  if(mode == SOUND) {
    display.print("  L:");
    display.print(soundLimit);
    if(soundLimit >= 100) // limit cannot go above 1000
      display.print(" ");
    else if (soundLimit < 100) // limit cannot go below 10
      display.print("  ");
  } else if(mode==ALARM) {
    display.print("   ");
    if(isAlarmOn)
      display.print("ON   ");
    else
      display.print("OFF  ");
  } else {
    display.print("        ");
  }
  // temperature text
  display.print(avgTemp);
  display.println(" C");

  // time text
  display.setCursor(0,11);
  display.setTextSize(3);
  display.setTextColor(SSD1306_WHITE);
  int display_h = now.hour();
  int display_m = now.minute();
  // display alarm time when in alarm mode
  if(mode == ALARM) {
    display_h = alarm_h;
    display_m = alarm_m;
  }
  if (display_h % 12 < 10) {
    if(display_h == 12 || display_h == 0)
      display.print("12");
    else {
      display.print(" ");
      display.print(display_h % 12);
    }
  }
  else {
    display.print(display_h % 12);
  } 
  display.print(":");
  if(display_m<10) {
    display.print("0");
    display.println(display_m);
  } else {
    display.println(display_m);
  }
  display.setTextSize(2);
  display.setCursor(display.width()-28,display.height()-14);
  if(display_h < 12)
    display.println("AM");
  else
    display.println("PM");
  display.drawCircle(display.width()-12, 2, 2, SSD1306_WHITE);
  display.display();
}

void changeAlarm() {
  CircuitPlayground.clearPixels();
  if (isLeftButtonPressed()) {
    if(alarm_h == 23)
      alarm_h = 0;
    else
      alarm_h++;
  }
  if (isRightButtonPressed()) {
    if(alarm_m == 59)
      alarm_m = 0;
    else
      alarm_m++;
  }
  if(isLightButtonPressed()) {
    isAlarmOn = !isAlarmOn;
  }
}

void changeSoundLimit() {
  if (isLeftButtonPressed()) {
    if(soundLimit > 10)
      soundLimit-=5;
    isLightOn = true;
  }
  if (isRightButtonPressed()) {
    if(soundLimit < 1000)
      soundLimit+=5;
    isLightOn = true;
  }
}

void setMode() {
  if(isModeButtonPressed()) {
    mode = static_cast<Modes>(mode+1);
    if(mode == END) {
      mode = MANUAL;
    }
    #if DEBUG
      Serial.println("TOGGLED MODE");
    #endif
  }
}

void setBrightness() {
  if(digitalRead(encoderPinB) == 1) {
    if(brightness < 255) {
      brightness+=5;
    }
    #if DEBUG
      Serial.println("CW");
    #endif
  } else {
    if(brightness > 0) {
      brightness-=5;
    }
    #if DEBUG
      Serial.println("CCW");
    #endif
  }
  CircuitPlayground.setBrightness(brightness);
}

void checkAlarm() {
  if(isAlarmOn && rtc.now().hour() == alarm_h && rtc.now().minute() == alarm_m) {
    CircuitPlayground.playTone(440,500);
    if(currLightButtonState == true) {
      isAlarmOn = false;
    }
  }
}

void changeTime() { // depreciated when using RTC
  CircuitPlayground.clearPixels();
  if (isLeftButtonPressed()) {
    adjustTime(3600);
  }
  if (isRightButtonPressed()) {
    adjustTime(60);
  }
}

void setup() {
  #if DEBUG
    Serial.begin(9600);
    Serial.println("Debug Mode");
  #endif
  CircuitPlayground.begin();
  if (! rtc.begin()) {
    #if DEBUG
      Serial.println("Couldn't find RTC");
      Serial.flush();
    #endif
//    while (1) delay(10);
  }
//  rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); // uncomment to set time
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    #if DEBUG
      Serial.println(F("SSD1306 allocation failed"));
    #endif
    for(;;); // Don't proceed, loop forever
  }
  display.display();
  display.clearDisplay();
  setTime(0,0,0,15,3,2022);
  pinMode(modeButtonPin, INPUT);
  pinMode(lightButtonPin, INPUT_PULLDOWN);
  pinMode(encoderPinA, INPUT);
  pinMode(encoderPinB, INPUT);
  attachInterrupt(digitalPinToInterrupt(encoderPinA), setBrightness, RISING);
  CircuitPlayground.setBrightness(brightness);
  currModeButtonState = digitalRead(modeButtonPin);
  currLightButtonState = digitalRead(lightButtonPin);
 }

void loop() {
  currModeButtonState = digitalRead(modeButtonPin);
  currLightButtonState = digitalRead(lightButtonPin);
  setMode();
  updateTemp();
  display_text();
  if(mode == MANUAL) {
    manualLight();
  } else if (mode == SOUND) {
    soundLight();
    changeSoundLimit();
  } else if (mode == ALARM) {
    changeAlarm();
  }
  if(mode != ALARM) {
    checkAlarm();
  }
  leftButtonState = CircuitPlayground.leftButton();
  rightButtonState = CircuitPlayground.rightButton();
  prevModeButtonState = currModeButtonState;
  prevLightButtonState = currLightButtonState;
}
