#include <Adafruit_ST7735.h>
#include <Adafruit_GFX.h>
#include "FluxGarage_RoboEyes-colored-st7735.h"

// Define pins for ST7735
#define TFT_CS    5
#define TFT_RST   4
#define TFT_DC    2

// Create an instance of the ST7735 class
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

// Create an instance of the RoboEyes (template-based)
RoboEyes<Adafruit_ST7735> eyes = RoboEyes<Adafruit_ST7735>(tft);

// Simple demo variables
unsigned long lastMoodChange = 0;
int currentMood = 0;
bool demoActive = true;

void setup() {
  Serial.begin(115200);
  Serial.println("ST7735 Simple Colored RoboEyes Demo");
  Serial.println("===================================");

  // Initialize the display
  Serial.println("Initializing ST7735 display...");
  tft.initR(INITR_BLACKTAB);
  tft.setRotation(0); // Portrait mode
  
  // Test display with a simple fill
  Serial.println("Testing display with white screen...");
  tft.fillScreen(0xFFFF); // White screen test
  delay(1000);
  
  Serial.println("Testing display with red screen...");
  tft.fillScreen(0xF800); // Red screen test
  delay(1000);
  
  Serial.println("Testing display with green screen...");
  tft.fillScreen(0x07E0); // Green screen test
  delay(1000);
  
  Serial.println("Testing display with blue screen...");
  tft.fillScreen(0x001F); // Blue screen test
  delay(1000);
  
  // Initialize RoboEyes with stable settings
  Serial.println("Initializing RoboEyes...");
  eyes.begin(128, 128, 60); // Full 160x128 display, 60 FPS for smooth display
  
  // Enable frame buffering for complete flicker elimination
  // Set to false if you want to use original display functions (may have some flicker but better shapes)
  eyes.enableFrameBuffer(true);
  
  // Set bright, visible colors (16-bit RGB565 format)
  eyes.setDisplayColors(0x0000, 0x07FF); // Black background, CYAN eyes
  
  // // Position eyes in the bottom area of the 160x128 display
  // // yOffset of +32 moves eyes from center (y=46) to bottom area (y=78)
  // eyes.setEyePositionOffset(32, 64);
  
  // Enable bounding box for debugging and positioning
  // eyes.setBoundingBox(true, 0xF800); // Red bounding box
  
  // Enable basic animations
  eyes.setAutoblinker(true, 3, 2); // Natural blinking
  eyes.setIdleMode(true, 5, 3); // Enable idle movement
  eyes.setCuriosity(true); // Enable curiosity effect
  
  // Set initial mood
  eyes.setMood(HAPPY);
  
  Serial.println("Display initialized with RoboEyes!");
  Serial.println("Frame buffering enabled for flicker-free rendering!");
  Serial.println("Eyes positioned in BOTTOM area of 160x128 display");
  Serial.println("Bounding box ENABLED for debugging and positioning");
  Serial.println("Eyes should now be visible as WHITE on BLACK background");
  delay(2000);
}

void loop() {
  // Update the eyes
  eyes.update();
  
  // Debug output every 5 seconds
  static unsigned long lastDebug = 0;
  if (millis() - lastDebug > 5000) {
    Serial.println("RoboEyes running - eyes should be visible");
    Serial.print("Current mood: ");
    Serial.println(currentMood);
    Serial.print("Frame buffer enabled: ");
    Serial.println(eyes.bufferEnabled ? "YES" : "NO");
    lastDebug = millis();
  }
  
  // Change mood every 10 seconds
  if (demoActive && millis() - lastMoodChange > 10000) {
    currentMood = (currentMood + 1) % 4; // Cycle through 4 moods
    changeMood(currentMood);
    lastMoodChange = millis();
  }
  
  delay(16); // Reduced delay for smoother 60 FPS animation
}

void changeMood(int mood) {
  switch (mood) {
    case 0:
      Serial.println("Mood: Happy");
      eyes.setMood(HAPPY);
      break;
    case 1:
      Serial.println("Mood: Tired");
      eyes.setMood(TIRED);
      break;
    case 2:
      Serial.println("Mood: Angry");
      eyes.setMood(ANGRY);
      break;
    case 3:
      Serial.println("Mood: Default");
      eyes.setMood(DEFAULT);
      break;
  }
}

// Optional: Add button control to stop/start demo
/*
void checkButton() {
  // Add button reading code here
  // if (digitalRead(BUTTON_PIN) == LOW) {
  //   demoActive = !demoActive;
  //   if (demoActive) {
  //     Serial.println("Demo resumed");
  //   } else {
  //     Serial.println("Demo paused");
  //   }
  // }
}
*/
