/*
 * FluxGarage RoboEyes for ST7735 TFT Displays V 1.1.1
 * Draws smoothly animated robot eyes on ST7735 TFT displays, based on the Adafruit GFX 
 * library's graphics primitives, such as rounded rectangles and triangles.
 *   
 * Copyright (C) 2024-2025 Dennis Hoelscher
 * www.fluxgarage.com
 * www.youtube.com/@FluxGarage
 *
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */


 #ifndef _FLUXGARAGE_ROBOEYES_ST7735_H
 #define _FLUXGARAGE_ROBOEYES_ST7735_H

 #include <Adafruit_ST7735.h>
 #include <Adafruit_GFX.h>


// Display colors - adapted for ST7735 (16-bit RGB565)
uint16_t BGCOLOR = 0x0000; // background and overlays (black)
uint16_t MAINCOLOR = 0x001F; // drawings (blue)
 
// For mood type switch
#define DEFAULT 0
#define TIRED 1
#define ANGRY 2
#define HAPPY 3

// For turning things on or off
#define ON 1
#define OFF 0

// For switch "predefined positions"
#define N 1 // north, top center
#define NE 2 // north-east, top right
#define E 3 // east, middle right
#define SE 4 // south-east, bottom right
#define S 5 // south, bottom center
#define SW 6 // south-west, bottom left
#define W 7 // west, middle left
#define NW 8 // north-west, top left 
// for middle center set "DEFAULT"


// Constructor: takes a reference to the ST7735 display object
// Eg: RoboEyes<Adafruit_ST7735> eyes = RoboEyes<Adafruit_ST7735>(tft);
template<typename AdafruitDisplay>
class RoboEyes
 {
 private:
 
 // Yes, everything is currently still accessible. Be responsible and don't mess things up :)
 
 public:
 
// Reference to Adafruit display object
AdafruitDisplay *display;

// For general setup - screen size and max. frame rate
int screenWidth = 128; // ST7735 display width, in pixels
int screenHeight = 160; // ST7735 display height, in pixels
int frameInterval = 20; // default value for 50 frames per second (1000/50 = 20 milliseconds)
unsigned long fpsTimer = 0; // for timing the frames per second

// For flicker-free rendering - track previous frame state
int prevEyeLx = 0, prevEyeLy = 0, prevEyeLwidth = 0, prevEyeLheight = 0;
int prevEyeRx = 0, prevEyeRy = 0, prevEyeRwidth = 0, prevEyeRheight = 0;
bool prevSweat = false;
bool firstFrame = true; // Flag to clear screen on first frame only

// Frame buffer for complete flicker-free rendering
uint16_t *frameBuffer = nullptr;
bool bufferEnabled = false;
bool bufferReady = false;

// Debug bounding box
bool showBoundingBox = false;
uint16_t boundingBoxColor = 0xF800; // Default red color
 
 // For controlling mood types and expressions
 bool tired = 0;
 bool angry = 0;
 bool happy = 0;
 bool curious = 0; // if true, draw the outer eye larger when looking left or right
 bool cyclops = 0; // if true, draw only one eye
 bool eyeL_open = 0; // left eye opened or closed?
 bool eyeR_open = 0; // right eye opened or closed?
 
 
 //*********************************************************************************************
 //  Eyes Geometry
 //*********************************************************************************************
 
// EYE LEFT - size and border radius
int eyeLwidthDefault = 36;
int eyeLheightDefault = 36;
int eyeLwidthCurrent = eyeLwidthDefault;
int eyeLheightCurrent = 1; // start with closed eye, otherwise set to eyeLheightDefault
int eyeLwidthNext = eyeLwidthDefault;
int eyeLheightNext = eyeLheightDefault;
int eyeLheightOffset = 0;
// Border Radius
byte eyeLborderRadiusDefault = 8;
byte eyeLborderRadiusCurrent = eyeLborderRadiusDefault;
byte eyeLborderRadiusNext = eyeLborderRadiusDefault;

// EYE RIGHT - size and border radius
int eyeRwidthDefault = eyeLwidthDefault;
int eyeRheightDefault = eyeLheightDefault;
int eyeRwidthCurrent = eyeRwidthDefault;
int eyeRheightCurrent = 1; // start with closed eye, otherwise set to eyeRheightDefault
int eyeRwidthNext = eyeRwidthDefault;
int eyeRheightNext = eyeRheightDefault;
int eyeRheightOffset = 0;
// Border Radius
byte eyeRborderRadiusDefault = 8;
byte eyeRborderRadiusCurrent = eyeRborderRadiusDefault;
byte eyeRborderRadiusNext = eyeRborderRadiusDefault;

// EYE LEFT - Coordinates
int eyeLxDefault = ((screenWidth)-(eyeLwidthDefault+spaceBetweenDefault+eyeRwidthDefault))/2;
int eyeLyDefault = ((screenHeight-eyeLheightDefault)/2);
int eyeLx = eyeLxDefault;
int eyeLy = eyeLyDefault;
int eyeLxNext = eyeLx;
int eyeLyNext = eyeLy;

// EYE RIGHT - Coordinates
int eyeRxDefault = eyeLx+eyeLwidthCurrent+spaceBetweenDefault;
int eyeRyDefault = eyeLy;
int eyeRx = eyeRxDefault;
int eyeRy = eyeRyDefault;
int eyeRxNext = eyeRx;
int eyeRyNext = eyeRy;
 
 // BOTH EYES 
 // Eyelid top size
 byte eyelidsHeightMax = eyeLheightDefault/2; // top eyelids max height
 byte eyelidsTiredHeight = 0;
 byte eyelidsTiredHeightNext = eyelidsTiredHeight;
 byte eyelidsAngryHeight = 0;
 byte eyelidsAngryHeightNext = eyelidsAngryHeight;
 // Bottom happy eyelids offset
 byte eyelidsHappyBottomOffsetMax = (eyeLheightDefault/2)+3;
 byte eyelidsHappyBottomOffset = 0;
 byte eyelidsHappyBottomOffsetNext = 0;
// Space between eyes
int spaceBetweenDefault = 10;
int spaceBetweenCurrent = spaceBetweenDefault;
int spaceBetweenNext = 10;
 
 
 //*********************************************************************************************
 //  Macro Animations
 //*********************************************************************************************
 
 // Animation - horizontal flicker/shiver
 bool hFlicker = 0;
 bool hFlickerAlternate = 0;
 byte hFlickerAmplitude = 2;
 
 // Animation - vertical flicker/shiver
 bool vFlicker = 0;
 bool vFlickerAlternate = 0;
 byte vFlickerAmplitude = 10;
 
 // Animation - auto blinking
 bool autoblinker = 0; // activate auto blink animation
 int blinkInterval = 1; // basic interval between each blink in full seconds
 int blinkIntervalVariation = 4; // interval variaton range in full seconds, random number inside of given range will be add to the basic blinkInterval, set to 0 for no variation
 unsigned long blinktimer = 0; // for organising eyeblink timing
 
 // Animation - idle mode: eyes looking in random directions
 bool idle = 0;
 int idleInterval = 1; // basic interval between each eye repositioning in full seconds
 int idleIntervalVariation = 3; // interval variaton range in full seconds, random number inside of given range will be add to the basic idleInterval, set to 0 for no variation
 unsigned long idleAnimationTimer = 0; // for organising eyeblink timing
 
 // Animation - eyes confused: eyes shaking left and right
 bool confused = 0;
 unsigned long confusedAnimationTimer = 0;
 int confusedAnimationDuration = 500;
 bool confusedToggle = 1;
 
 // Animation - eyes laughing: eyes shaking up and down
 bool laugh = 0;
 unsigned long laughAnimationTimer = 0;
 int laughAnimationDuration = 500;
 bool laughToggle = 1;
 
// Animation - sweat on the forehead
bool sweat = 0;
byte sweatBorderradius = 3;
 
 // Sweat drop 1
 int sweat1XPosInitial = 2;
 int sweat1XPos;
 float sweat1YPos = 2;
 int sweat1YPosMax;
 float sweat1Height = 2;
 float sweat1Width = 1;
 
 // Sweat drop 2
 int sweat2XPosInitial = 2;
 int sweat2XPos;
 float sweat2YPos = 2;
 int sweat2YPosMax;
 float sweat2Height = 2;
 float sweat2Width = 1;
 
 // Sweat drop 3
 int sweat3XPosInitial = 2;
 int sweat3XPos;
 float sweat3YPos = 2;
 int sweat3YPosMax;
 float sweat3Height = 2;
 float sweat3Width = 1;
 
 
 //*********************************************************************************************
 //  GENERAL METHODS
 //*********************************************************************************************
 
 RoboEyes(AdafruitDisplay &disp) : display(&disp) {};

// Startup RoboEyes with defined screen-width, screen-height and max. frames per second
void begin(int width, int height, byte frameRate) {
     screenWidth = width; // ST7735 display width, in pixels
     screenHeight = height; // ST7735 display height, in pixels
   display->fillScreen(BGCOLOR); // clear the display with background color
   eyeLheightCurrent = 1; // start with closed eyes
   eyeRheightCurrent = 1; // start with closed eyes
   setFramerate(frameRate); // calculate frame interval based on defined frameRate
   
   // Reset tracking variables for flicker-free rendering
   firstFrame = true;
   prevEyeLx = 0; prevEyeLy = 0; prevEyeLwidth = 0; prevEyeLheight = 0;
   prevEyeRx = 0; prevEyeRy = 0; prevEyeRwidth = 0; prevEyeRheight = 0;
   prevSweat = false;
   
   // Initialize frame buffer if not already done
   if(!frameBuffer) {
     frameBuffer = (uint16_t*)malloc(screenWidth * screenHeight * sizeof(uint16_t));
     if(frameBuffer) {
       bufferEnabled = true;
       clearFrameBuffer();
     }
   }
 }

// Enable or disable frame buffering for complete flicker elimination
void enableFrameBuffer(bool enable) {
  if(enable && !frameBuffer) {
    frameBuffer = (uint16_t*)malloc(screenWidth * screenHeight * sizeof(uint16_t));
    if(frameBuffer) {
      bufferEnabled = true;
      clearFrameBuffer();
    }
  } else if(!enable && frameBuffer) {
    free(frameBuffer);
    frameBuffer = nullptr;
    bufferEnabled = false;
  }
}

// Clear the frame buffer with background color
void clearFrameBuffer() {
  if(frameBuffer) {
    // Use memset for faster clearing (if BGCOLOR is 0)
    if(BGCOLOR == 0) {
      memset(frameBuffer, 0, screenWidth * screenHeight * sizeof(uint16_t));
    } else {
      // Fallback to loop for non-zero background colors
      for(int i = 0; i < screenWidth * screenHeight; i++) {
        frameBuffer[i] = BGCOLOR;
      }
    }
  }
}

// Draw a pixel to the frame buffer
void drawPixelToBuffer(int x, int y, uint16_t color) {
  if(frameBuffer && x >= 0 && x < screenWidth && y >= 0 && y < screenHeight) {
    frameBuffer[y * screenWidth + x] = color;
  }
}

// Draw a filled rectangle to the frame buffer
void drawRectToBuffer(int x, int y, int w, int h, uint16_t color) {
  if(frameBuffer) {
    for(int py = y; py < y + h; py++) {
      for(int px = x; px < x + w; px++) {
        if(px >= 0 && px < screenWidth && py >= 0 && py < screenHeight) {
          frameBuffer[py * screenWidth + px] = color;
        }
      }
    }
  }
}

// Draw a filled round rectangle to the frame buffer
void drawRoundRectToBuffer(int x, int y, int w, int h, int r, uint16_t color) {
  if(frameBuffer && r > 0) {
    // Ensure radius doesn't exceed half the smaller dimension
    r = min(r, min(w/2, h/2));
    
    // Draw the main rectangle body (center area)
    if(w > 2*r && h > 0) {
      drawRectToBuffer(x + r, y, w - 2*r, h, color);
    }
    
    // Draw the left and right side rectangles
    if(h > 2*r && r > 0) {
      drawRectToBuffer(x, y + r, r, h - 2*r, color);
      drawRectToBuffer(x + w - r, y + r, r, h - 2*r, color);
    }
    
    // Draw the rounded corners using a more accurate circle algorithm
    for(int i = 0; i < r; i++) {
      for(int j = 0; j < r; j++) {
        // Use distance from corner to determine if pixel should be filled
        int dx = r - 1 - i;
        int dy = r - 1 - j;
        int distSquared = dx*dx + dy*dy;
        int radiusSquared = r*r;
        
        // Fill pixel if it's inside the circle
        if(distSquared <= radiusSquared) {
          // Top-left corner
          int px = x + i;
          int py = y + j;
          if(px >= 0 && px < screenWidth && py >= 0 && py < screenHeight) {
            frameBuffer[py * screenWidth + px] = color;
          }
          
          // Top-right corner
          px = x + w - 1 - i;
          py = y + j;
          if(px >= 0 && px < screenWidth && py >= 0 && py < screenHeight) {
            frameBuffer[py * screenWidth + px] = color;
          }
          
          // Bottom-left corner
          px = x + i;
          py = y + h - 1 - j;
          if(px >= 0 && px < screenWidth && py >= 0 && py < screenHeight) {
            frameBuffer[py * screenWidth + px] = color;
          }
          
          // Bottom-right corner
          px = x + w - 1 - i;
          py = y + h - 1 - j;
          if(px >= 0 && px < screenWidth && py >= 0 && py < screenHeight) {
            frameBuffer[py * screenWidth + px] = color;
          }
        }
      }
    }
  } else {
    // Fallback to regular rectangle if no radius
    drawRectToBuffer(x, y, w, h, color);
  }
}

// Draw a filled triangle to the frame buffer
void drawTriangleToBuffer(int x0, int y0, int x1, int y1, int x2, int y2, uint16_t color) {
  if(frameBuffer) {
    int minX = max(0, min(x0, min(x1, x2)));
    int maxX = min(screenWidth - 1, max(x0, max(x1, x2)));
    int minY = max(0, min(y0, min(y1, y2)));
    int maxY = min(screenHeight - 1, max(y0, max(y1, y2)));
    
    for(int py = minY; py <= maxY; py++) {
      for(int px = minX; px <= maxX; px++) {
        if(isPointInTriangle(px, py, x0, y0, x1, y1, x2, y2)) {
          frameBuffer[py * screenWidth + px] = color;
        }
      }
    }
  }
}

// Helper function to check if point is inside triangle (using barycentric coordinates)
bool isPointInTriangle(int px, int py, int x0, int y0, int x1, int y1, int x2, int y2) {
  // Calculate barycentric coordinates
  int denom = (y1 - y2) * (x0 - x2) + (x2 - x1) * (y0 - y2);
  if(denom == 0) return false; // Degenerate triangle
  
  int a = ((y1 - y2) * (px - x2) + (x2 - x1) * (py - y2));
  int b = ((y2 - y0) * (px - x2) + (x0 - x2) * (py - y2));
  
  // Use integer arithmetic to avoid floating point issues
  if(denom > 0) {
    return (a >= 0 && b >= 0 && (a + b) <= denom);
  } else {
    return (a <= 0 && b <= 0 && (a + b) >= denom);
  }
}

// Flush the frame buffer to the display
void flushFrameBuffer() {
  if(frameBuffer && bufferEnabled) {
    display->drawRGBBitmap(0, 0, frameBuffer, screenWidth, screenHeight);
    bufferReady = false;
  }
}
 
void update(){
  // Limit drawing updates to defined max framerate
  if(millis()-fpsTimer >= frameInterval){
    drawEyes();
    
    // Flush frame buffer to display if buffering is enabled and buffer is ready
    if(bufferEnabled && bufferReady) {
      flushFrameBuffer();
    }
    
    fpsTimer = millis();
  }
}
 
 
 //*********************************************************************************************
 //  SETTERS METHODS
 //*********************************************************************************************
 
 // Calculate frame interval based on defined frameRate
 void setFramerate(byte fps){
   frameInterval = 1000/fps;
 }
 
// Set color values
void setDisplayColors(uint16_t background, uint16_t main) {
  BGCOLOR = background; // background and overlays, choose 0x0000 for black and 0xFFFF for white
  MAINCOLOR = main; // drawings, choose 0x001F for blue, 0xF800 for red, 0x07E0 for green, etc.
}
 
 void setWidth(byte leftEye, byte rightEye) {
     eyeLwidthNext = leftEye;
     eyeRwidthNext = rightEye;
   eyeLwidthDefault = leftEye;
   eyeRwidthDefault = rightEye;
 }
 
 void setHeight(byte leftEye, byte rightEye) {
     eyeLheightNext = leftEye;
     eyeRheightNext = rightEye;
   eyeLheightDefault = leftEye;
   eyeRheightDefault = rightEye;
 }
 
 // Set border radius for left and right eye
 void setBorderradius(byte leftEye, byte rightEye) {
     eyeLborderRadiusNext = leftEye;
     eyeRborderRadiusNext = rightEye;
   eyeLborderRadiusDefault = leftEye;
   eyeRborderRadiusDefault = rightEye;
 }
 
 // Set space between the eyes, can also be negative
 void setSpacebetween(int space) {
   spaceBetweenNext = space;
   spaceBetweenDefault = space;
 }
 
 // Set eye positioning offset (useful for positioning eyes in different areas of screen)
 void setEyePositionOffset(int xOffset, int yOffset) {
   eyeLxDefault = ((screenWidth)-(eyeLwidthDefault+spaceBetweenDefault+eyeRwidthDefault))/2 + xOffset;
   eyeLyDefault = ((screenHeight-eyeLheightDefault)/2) + yOffset;
   eyeLx = eyeLxDefault;
   eyeLy = eyeLyDefault;
   eyeLxNext = eyeLx;
   eyeLyNext = eyeLy;
   
   // Update right eye position
   eyeRxDefault = eyeLx+eyeLwidthCurrent+spaceBetweenDefault;
   eyeRyDefault = eyeLy;
   eyeRx = eyeRxDefault;
   eyeRy = eyeRyDefault;
   eyeRxNext = eyeRx;
   eyeRyNext = eyeRy;
 }
 
 // Draw bounding box for debugging and positioning
 void drawBoundingBox(uint16_t color = 0xF800) { // Default red color
   if (bufferEnabled) {
     // Draw frame buffer bounding box
     drawRectToBuffer(0, 0, screenWidth, 2, color); // Top border
     drawRectToBuffer(0, screenHeight-2, screenWidth, 2, color); // Bottom border
     drawRectToBuffer(0, 0, 2, screenHeight, color); // Left border
     drawRectToBuffer(screenWidth-2, 0, 2, screenHeight, color); // Right border
     
     // Draw center crosshair
     drawRectToBuffer(screenWidth/2-1, 0, 2, screenHeight, color); // Vertical center line
     drawRectToBuffer(0, screenHeight/2-1, screenWidth, 2, color); // Horizontal center line
     
     // Draw eye area bounding box
     int eyeAreaLeft = eyeLx - 5;
     int eyeAreaTop = eyeLy - 5;
     int eyeAreaRight = eyeRx + eyeRwidthCurrent + 5;
     int eyeAreaBottom = eyeRy + eyeRheightCurrent + 5;
     
     // Draw eye area border
     drawRectToBuffer(eyeAreaLeft, eyeAreaTop, eyeAreaRight - eyeAreaLeft, 2, color); // Top
     drawRectToBuffer(eyeAreaLeft, eyeAreaBottom, eyeAreaRight - eyeAreaLeft, 2, color); // Bottom
     drawRectToBuffer(eyeAreaLeft, eyeAreaTop, 2, eyeAreaBottom - eyeAreaTop, color); // Left
     drawRectToBuffer(eyeAreaRight, eyeAreaTop, 2, eyeAreaBottom - eyeAreaTop, color); // Right
   } else {
     // Draw direct display bounding box
     display->fillRect(0, 0, screenWidth, 2, color); // Top border
     display->fillRect(0, screenHeight-2, screenWidth, 2, color); // Bottom border
     display->fillRect(0, 0, 2, screenHeight, color); // Left border
     display->fillRect(screenWidth-2, 0, 2, screenHeight, color); // Right border
     
     // Draw center crosshair
     display->fillRect(screenWidth/2-1, 0, 2, screenHeight, color); // Vertical center line
     display->fillRect(0, screenHeight/2-1, screenWidth, 2, color); // Horizontal center line
     
     // Draw eye area bounding box
     int eyeAreaLeft = eyeLx - 5;
     int eyeAreaTop = eyeLy - 5;
     int eyeAreaRight = eyeRx + eyeRwidthCurrent + 5;
     int eyeAreaBottom = eyeRy + eyeRheightCurrent + 5;
     
     // Draw eye area border
     display->fillRect(eyeAreaLeft, eyeAreaTop, eyeAreaRight - eyeAreaLeft, 2, color); // Top
     display->fillRect(eyeAreaLeft, eyeAreaBottom, eyeAreaRight - eyeAreaLeft, 2, color); // Bottom
     display->fillRect(eyeAreaLeft, eyeAreaTop, 2, eyeAreaBottom - eyeAreaTop, color); // Left
     display->fillRect(eyeAreaRight, eyeAreaTop, 2, eyeAreaBottom - eyeAreaTop, color); // Right
   }
 }
 
// Set mood expression
void setMood(unsigned char mood)
  {
    switch (mood)
    {
    case TIRED:
      tired=1; 
      angry=0; 
      happy=0;
      break;
    case ANGRY:
      tired=0; 
      angry=1; 
      happy=0;
      break;
    case HAPPY:
      tired=0; 
      angry=0; 
      happy=1;
      break;
    default:
      tired=0; 
      angry=0; 
      happy=0;
      break;
    }
  }
 
 // Set predefined position
 void setPosition(unsigned char position)
   {
     switch (position)
     {
     case N:
       // North, top center
       eyeLxNext = getScreenConstraint_X()/2;
       eyeLyNext = 0;
       break;
     case NE:
       // North-east, top right
       eyeLxNext = getScreenConstraint_X();
       eyeLyNext = 0;
       break;
     case E:
       // East, middle right
       eyeLxNext = getScreenConstraint_X();
       eyeLyNext = getScreenConstraint_Y()/2;
       break;
     case SE:
       // South-east, bottom right
       eyeLxNext = getScreenConstraint_X();
       eyeLyNext = getScreenConstraint_Y();
       break;
     case S:
       // South, bottom center
       eyeLxNext = getScreenConstraint_X()/2;
       eyeLyNext = getScreenConstraint_Y();
       break;
     case SW:
       // South-west, bottom left
       eyeLxNext = 0;
       eyeLyNext = getScreenConstraint_Y();
       break;
     case W:
       // West, middle left
       eyeLxNext = 0;
       eyeLyNext = getScreenConstraint_Y()/2;
       break;
     case NW:
       // North-west, top left
       eyeLxNext = 0;
       eyeLyNext = 0;
       break;
     default:
       // Middle center
       eyeLxNext = getScreenConstraint_X()/2;
       eyeLyNext = getScreenConstraint_Y()/2;
       break;
     }
   }
 
 // Set automated eye blinking, minimal blink interval in full seconds and blink interval variation range in full seconds
 void setAutoblinker(bool active, int interval, int variation){
   autoblinker = active;
   blinkInterval = interval;
   blinkIntervalVariation = variation;
 }
 void setAutoblinker(bool active){
   autoblinker = active;
 }
 
 // Set idle mode - automated eye repositioning, minimal time interval in full seconds and time interval variation range in full seconds
 void setIdleMode(bool active, int interval, int variation){
   idle = active;
   idleInterval = interval;
   idleIntervalVariation = variation;
 }
 void setIdleMode(bool active) {
   idle = active;
 }
 
 // Set curious mode - the respectively outer eye gets larger when looking left or right
 void setCuriosity(bool curiousBit) {
   curious = curiousBit;
 }
 
 // Set cyclops mode - show only one eye 
 void setCyclops(bool cyclopsBit) {
   cyclops = cyclopsBit;
 }
 
 // Set horizontal flickering (displacing eyes left/right)
 void setHFlicker (bool flickerBit, byte Amplitude) {
   hFlicker = flickerBit; // turn flicker on or off
   hFlickerAmplitude = Amplitude; // define amplitude of flickering in pixels
 }
 void setHFlicker (bool flickerBit) {
   hFlicker = flickerBit; // turn flicker on or off
 }
 
 // Set vertical flickering (displacing eyes up/down)
 void setVFlicker (bool flickerBit, byte Amplitude) {
   vFlicker = flickerBit; // turn flicker on or off
   vFlickerAmplitude = Amplitude; // define amplitude of flickering in pixels
 }
 void setVFlicker (bool flickerBit) {
   vFlicker = flickerBit; // turn flicker on or off
 }
 
 void setSweat (bool sweatBit) {
   sweat = sweatBit; // turn sweat on or off
 }
 
 // Enable/disable bounding box for debugging
 void setBoundingBox(bool enabled, uint16_t color = 0xF800) {
   showBoundingBox = enabled;
   boundingBoxColor = color;
 }
 
 
 //*********************************************************************************************
 //  GETTERS METHODS
 //*********************************************************************************************
 
// Returns the max x position for left eye
int getScreenConstraint_X(){
  return screenWidth-eyeLwidthCurrent-spaceBetweenCurrent-eyeRwidthCurrent;
} 

// Returns the max y position for left eye
int getScreenConstraint_Y(){
 return screenHeight-eyeLheightDefault; // using default height here, because height will vary when blinking and in curious mode
}
 
 
 //*********************************************************************************************
 //  BASIC ANIMATION METHODS
 //*********************************************************************************************
 
 // BLINKING FOR BOTH EYES AT ONCE
 // Close both eyes
 void close() {
     eyeLheightNext = 1; // closing left eye
   eyeRheightNext = 1; // closing right eye
   eyeL_open = 0; // left eye not opened (=closed)
     eyeR_open = 0; // right eye not opened (=closed)
 }
 
 // Open both eyes
 void open() {
   eyeL_open = 1; // left eye opened - if true, drawEyes() will take care of opening eyes again
     eyeR_open = 1; // right eye opened
 }
 
 // Trigger eyeblink animation
 void blink() {
   close();
   open();
 }
 
 // BLINKING FOR SINGLE EYES, CONTROL EACH EYE SEPARATELY
 // Close eye(s)
 void close(bool left, bool right) {
   if(left){
     eyeLheightNext = 1; // blinking left eye
     eyeL_open = 0; // left eye not opened (=closed)
   }
   if(right){
       eyeRheightNext = 1; // blinking right eye
       eyeR_open = 0; // right eye not opened (=closed)
   }
 }
 
 // Open eye(s)
 void open(bool left, bool right) {
   if(left){
     eyeL_open = 1; // left eye opened - if true, drawEyes() will take care of opening eyes again
   }
   if(right){
     eyeR_open = 1; // right eye opened
   }
 }
 
 // Trigger eyeblink(s) animation
 void blink(bool left, bool right) {
   close(left, right);
   open(left, right);
 }
 
 
 //*********************************************************************************************
 //  MACRO ANIMATION METHODS
 //*********************************************************************************************
 
 // Play confused animation - one shot animation of eyes shaking left and right
 void anim_confused() {
     confused = 1;
 }
 
 // Play laugh animation - one shot animation of eyes shaking up and down
 void anim_laugh() {
   laugh = 1;
 }
 
 //*********************************************************************************************
 //  PRE-CALCULATIONS AND ACTUAL DRAWINGS
 //*********************************************************************************************
 
 void drawEyes(){

   //// PRE-CALCULATIONS - EYE SIZES AND VALUES FOR ANIMATION TWEENINGS ////

   // Vertical size offset for larger eyes when looking left or right (curious gaze)
   if(curious){
     if(eyeLxNext<=10){eyeLheightOffset=8;}
     else if (eyeLxNext>=(getScreenConstraint_X()-10) && cyclops){eyeLheightOffset=8;}
     else{eyeLheightOffset=0;} // left eye
     if(eyeRxNext>=screenWidth-eyeRwidthCurrent-10){eyeRheightOffset=8;}
     else{eyeRheightOffset=0;} // right eye
   } else {
     eyeLheightOffset=0; // reset height offset for left eye
     eyeRheightOffset=0; // reset height offset for right eye
   }

   // Left eye height
   eyeLheightCurrent = (eyeLheightCurrent + eyeLheightNext + eyeLheightOffset)/2;
   eyeLy+= ((eyeLheightDefault-eyeLheightCurrent)/2); // vertical centering of eye when closing
   eyeLy-= eyeLheightOffset/2;
   // Right eye height
   eyeRheightCurrent = (eyeRheightCurrent + eyeRheightNext + eyeRheightOffset)/2;
   eyeRy+= (eyeRheightDefault-eyeRheightCurrent)/2; // vertical centering of eye when closing
   eyeRy-= eyeRheightOffset/2;


   // Open eyes again after closing them
     if(eyeL_open){
       if(eyeLheightCurrent <= 1 + eyeLheightOffset){eyeLheightNext = eyeLheightDefault;} 
   }
   if(eyeR_open){
       if(eyeRheightCurrent <= 1 + eyeRheightOffset){eyeRheightNext = eyeRheightDefault;} 
   }

   // Left eye width
   eyeLwidthCurrent = (eyeLwidthCurrent + eyeLwidthNext)/2;
   // Right eye width
   eyeRwidthCurrent = (eyeRwidthCurrent + eyeRwidthNext)/2;


   // Space between eyes
   spaceBetweenCurrent = (spaceBetweenCurrent + spaceBetweenNext)/2;

   // Left eye coordinates
   eyeLx = (eyeLx + eyeLxNext)/2;
   eyeLy = (eyeLy + eyeLyNext)/2;
   // Right eye coordinates
   eyeRxNext = eyeLxNext+eyeLwidthCurrent+spaceBetweenCurrent; // right eye's x position depends on left eyes position + the space between
   eyeRyNext = eyeLyNext; // right eye's y position should be the same as for the left eye
   eyeRx = (eyeRx + eyeRxNext)/2;
   eyeRy = (eyeRy + eyeRyNext)/2;

   // Left eye border radius
   eyeLborderRadiusCurrent = (eyeLborderRadiusCurrent + eyeLborderRadiusNext)/2;
   // Right eye border radius
   eyeRborderRadiusCurrent = (eyeRborderRadiusCurrent + eyeRborderRadiusNext)/2;
   

   //// APPLYING MACRO ANIMATIONS ////

     if(autoblinker){
         if(millis() >= blinktimer){
         blink();
         blinktimer = millis()+(blinkInterval*1000)+(random(blinkIntervalVariation)*1000); // calculate next time for blinking
         }
     }

   // Laughing - eyes shaking up and down for the duration defined by laughAnimationDuration (default = 500ms)
   if(laugh){
     if(laughToggle){
       setVFlicker(1, 5);
       laughAnimationTimer = millis();
       laughToggle = 0;
     } else if(millis() >= laughAnimationTimer+laughAnimationDuration){
       setVFlicker(0, 0);
       laughToggle = 1;
       laugh=0; 
     }
   }

   // Confused - eyes shaking left and right for the duration defined by confusedAnimationDuration (default = 500ms)
   if(confused){
     if(confusedToggle){
       setHFlicker(1, 20);
       confusedAnimationTimer = millis();
       confusedToggle = 0;
     } else if(millis() >= confusedAnimationTimer+confusedAnimationDuration){
       setHFlicker(0, 0);
       confusedToggle = 1;
       confused=0; 
     }
   }

   // Idle - eyes moving to random positions on screen
   if(idle){
     if(millis() >= idleAnimationTimer){
       eyeLxNext = random(getScreenConstraint_X());
       eyeLyNext = random(getScreenConstraint_Y());
       idleAnimationTimer = millis()+(idleInterval*1000)+(random(idleIntervalVariation)*1000); // calculate next time for eyes repositioning
     }
   }

   // Adding offsets for horizontal flickering/shivering
   if(hFlicker){
     if(hFlickerAlternate) {
       eyeLx += hFlickerAmplitude;
       eyeRx += hFlickerAmplitude;
     } else {
       eyeLx -= hFlickerAmplitude;
       eyeRx -= hFlickerAmplitude;
     }
     hFlickerAlternate = !hFlickerAlternate;
   }

   // Adding offsets for horizontal flickering/shivering
   if(vFlicker){
     if(vFlickerAlternate) {
       eyeLy += vFlickerAmplitude;
       eyeRy += vFlickerAmplitude;
     } else {
       eyeLy -= vFlickerAmplitude;
       eyeRy -= vFlickerAmplitude;
     }
     vFlickerAlternate = !vFlickerAlternate;
   }

   // Cyclops mode, set second eye's size and space between to 0
   if(cyclops){
     eyeRwidthCurrent = 0;
     eyeRheightCurrent = 0;
     spaceBetweenCurrent = 0;
   }

   //// ACTUAL DRAWINGS ////

   // Use frame buffer for complete flicker elimination
   if (bufferEnabled) {
     // Clear frame buffer
     clearFrameBuffer();
     
     // Draw basic eye rectangles to buffer
     drawRoundRectToBuffer(eyeLx, eyeLy, eyeLwidthCurrent, eyeLheightCurrent, eyeLborderRadiusCurrent, MAINCOLOR); // left eye
     if (!cyclops){
       drawRoundRectToBuffer(eyeRx, eyeRy, eyeRwidthCurrent, eyeRheightCurrent, eyeRborderRadiusCurrent, MAINCOLOR); // right eye
     }
     
     bufferReady = true; // Mark buffer as ready for display
   } else {
     // Fallback to selective clearing (original method)
     if (firstFrame) {
       display->fillScreen(BGCOLOR);
       firstFrame = false;
     } else {
       // Clear only the areas that need updating to prevent flicker
       
       // Clear previous left eye area if it has changed
       if (prevEyeLx != eyeLx || prevEyeLy != eyeLy || prevEyeLwidth != eyeLwidthCurrent || prevEyeLheight != eyeLheightCurrent) {
         display->fillRoundRect(prevEyeLx-2, prevEyeLy-2, prevEyeLwidth+4, prevEyeLheight+4, eyeLborderRadiusCurrent+2, BGCOLOR);
       }
       
       // Clear previous right eye area if it has changed (and not cyclops)
       if (!cyclops && (prevEyeRx != eyeRx || prevEyeRy != eyeRy || prevEyeRwidth != eyeRwidthCurrent || prevEyeRheight != eyeRheightCurrent)) {
         display->fillRoundRect(prevEyeRx-2, prevEyeRy-2, prevEyeRwidth+4, prevEyeRheight+4, eyeRborderRadiusCurrent+2, BGCOLOR);
       }
       
       // Clear sweat drop area if sweat state changed
       if (prevSweat != sweat) {
         if (prevSweat) {
           display->fillRoundRect(0, 0, screenWidth, 25, 0, BGCOLOR); // Clear top area if sweat was active
         }
       }
     }

     // Draw basic eye rectangles directly to display
     display->fillRoundRect(eyeLx, eyeLy, eyeLwidthCurrent, eyeLheightCurrent, eyeLborderRadiusCurrent, MAINCOLOR); // left eye
     if (!cyclops){
       display->fillRoundRect(eyeRx, eyeRy, eyeRwidthCurrent, eyeRheightCurrent, eyeRborderRadiusCurrent, MAINCOLOR); // right eye
     }
   }

   // Prepare mood type transitions
   if (tired){eyelidsTiredHeightNext = eyeLheightCurrent/2; eyelidsAngryHeightNext = 0;} else{eyelidsTiredHeightNext = 0;}
   if (angry){eyelidsAngryHeightNext = eyeLheightCurrent/2; eyelidsTiredHeightNext = 0;} else{eyelidsAngryHeightNext = 0;}
   if (happy){eyelidsHappyBottomOffsetNext = eyeLheightCurrent/2;} else{eyelidsHappyBottomOffsetNext = 0;}

   // Draw tired top eyelids 
     eyelidsTiredHeight = (eyelidsTiredHeight + eyelidsTiredHeightNext)/2;
     if (bufferEnabled) {
       if (!cyclops){
         drawTriangleToBuffer(eyeLx, eyeLy-1, eyeLx+eyeLwidthCurrent, eyeLy-1, eyeLx, eyeLy+eyelidsTiredHeight-1, BGCOLOR); // left eye 
         drawTriangleToBuffer(eyeRx, eyeRy-1, eyeRx+eyeRwidthCurrent, eyeRy-1, eyeRx+eyeRwidthCurrent, eyeRy+eyelidsTiredHeight-1, BGCOLOR); // right eye
       } else {
         // Cyclops tired eyelids
         drawTriangleToBuffer(eyeLx, eyeLy-1, eyeLx+(eyeLwidthCurrent/2), eyeLy-1, eyeLx, eyeLy+eyelidsTiredHeight-1, BGCOLOR); // left eyelid half
         drawTriangleToBuffer(eyeLx+(eyeLwidthCurrent/2), eyeLy-1, eyeLx+eyeLwidthCurrent, eyeLy-1, eyeLx+eyeLwidthCurrent, eyeLy+eyelidsTiredHeight-1, BGCOLOR); // right eyelid half
       }
     } else {
       if (!cyclops){
         display->fillTriangle(eyeLx, eyeLy-1, eyeLx+eyeLwidthCurrent, eyeLy-1, eyeLx, eyeLy+eyelidsTiredHeight-1, BGCOLOR); // left eye 
         display->fillTriangle(eyeRx, eyeRy-1, eyeRx+eyeRwidthCurrent, eyeRy-1, eyeRx+eyeRwidthCurrent, eyeRy+eyelidsTiredHeight-1, BGCOLOR); // right eye
       } else {
         // Cyclops tired eyelids
         display->fillTriangle(eyeLx, eyeLy-1, eyeLx+(eyeLwidthCurrent/2), eyeLy-1, eyeLx, eyeLy+eyelidsTiredHeight-1, BGCOLOR); // left eyelid half
         display->fillTriangle(eyeLx+(eyeLwidthCurrent/2), eyeLy-1, eyeLx+eyeLwidthCurrent, eyeLy-1, eyeLx+eyeLwidthCurrent, eyeLy+eyelidsTiredHeight-1, BGCOLOR); // right eyelid half
       }
     }

   // Draw angry top eyelids 
     eyelidsAngryHeight = (eyelidsAngryHeight + eyelidsAngryHeightNext)/2;
     if (bufferEnabled) {
       if (!cyclops){ 
         drawTriangleToBuffer(eyeLx, eyeLy-1, eyeLx+eyeLwidthCurrent, eyeLy-1, eyeLx+eyeLwidthCurrent, eyeLy+eyelidsAngryHeight-1, BGCOLOR); // left eye
         drawTriangleToBuffer(eyeRx, eyeRy-1, eyeRx+eyeRwidthCurrent, eyeRy-1, eyeRx, eyeRy+eyelidsAngryHeight-1, BGCOLOR); // right eye
       } else {
         // Cyclops angry eyelids
         drawTriangleToBuffer(eyeLx, eyeLy-1, eyeLx+(eyeLwidthCurrent/2), eyeLy-1, eyeLx+(eyeLwidthCurrent/2), eyeLy+eyelidsAngryHeight-1, BGCOLOR); // left eyelid half
         drawTriangleToBuffer(eyeLx+(eyeLwidthCurrent/2), eyeLy-1, eyeLx+eyeLwidthCurrent, eyeLy-1, eyeLx+(eyeLwidthCurrent/2), eyeLy+eyelidsAngryHeight-1, BGCOLOR); // right eyelid half
       }
     } else {
       if (!cyclops){ 
         display->fillTriangle(eyeLx, eyeLy-1, eyeLx+eyeLwidthCurrent, eyeLy-1, eyeLx+eyeLwidthCurrent, eyeLy+eyelidsAngryHeight-1, BGCOLOR); // left eye
         display->fillTriangle(eyeRx, eyeRy-1, eyeRx+eyeRwidthCurrent, eyeRy-1, eyeRx, eyeRy+eyelidsAngryHeight-1, BGCOLOR); // right eye
       } else {
         // Cyclops angry eyelids
         display->fillTriangle(eyeLx, eyeLy-1, eyeLx+(eyeLwidthCurrent/2), eyeLy-1, eyeLx+(eyeLwidthCurrent/2), eyeLy+eyelidsAngryHeight-1, BGCOLOR); // left eyelid half
         display->fillTriangle(eyeLx+(eyeLwidthCurrent/2), eyeLy-1, eyeLx+eyeLwidthCurrent, eyeLy-1, eyeLx+(eyeLwidthCurrent/2), eyeLy+eyelidsAngryHeight-1, BGCOLOR); // right eyelid half
       }
     }

   // Draw happy bottom eyelids
     eyelidsHappyBottomOffset = (eyelidsHappyBottomOffset + eyelidsHappyBottomOffsetNext)/2;
     if (bufferEnabled) {
       drawRoundRectToBuffer(eyeLx-1, (eyeLy+eyeLheightCurrent)-eyelidsHappyBottomOffset+1, eyeLwidthCurrent+2, eyeLheightDefault, eyeLborderRadiusCurrent, BGCOLOR); // left eye
       if (!cyclops){ 
         drawRoundRectToBuffer(eyeRx-1, (eyeRy+eyeRheightCurrent)-eyelidsHappyBottomOffset+1, eyeRwidthCurrent+2, eyeRheightDefault, eyeRborderRadiusCurrent, BGCOLOR); // right eye
       }
     } else {
       display->fillRoundRect(eyeLx-1, (eyeLy+eyeLheightCurrent)-eyelidsHappyBottomOffset+1, eyeLwidthCurrent+2, eyeLheightDefault, eyeLborderRadiusCurrent, BGCOLOR); // left eye
       if (!cyclops){ 
         display->fillRoundRect(eyeRx-1, (eyeRy+eyeRheightCurrent)-eyelidsHappyBottomOffset+1, eyeRwidthCurrent+2, eyeRheightDefault, eyeRborderRadiusCurrent, BGCOLOR); // right eye
       }
     }

   // Add sweat drops
     if (sweat){
       // Sweat drop 1 -> left corner
       if(sweat1YPos <= sweat1YPosMax){sweat1YPos+=0.5;} // vertical movement from initial to max
       else {sweat1XPosInitial = random(30); sweat1YPos = 2; sweat1YPosMax = (random(10)+10); sweat1Width = 1; sweat1Height = 2;} // if max vertical position is reached: reset all values for next drop
       if(sweat1YPos <= sweat1YPosMax/2){sweat1Width+=0.5; sweat1Height+=0.5;} // shape grows in first half of animation ...
       else {sweat1Width-=0.1; sweat1Height-=0.5;} // ... and shrinks in second half of animation
       sweat1XPos = sweat1XPosInitial-(sweat1Width/2); // keep the growing shape centered to initial x position
       if (bufferEnabled) {
         drawRoundRectToBuffer(sweat1XPos, sweat1YPos, sweat1Width, sweat1Height, sweatBorderradius, MAINCOLOR); // draw sweat drop
       } else {
         display->fillRoundRect(sweat1XPos, sweat1YPos, sweat1Width, sweat1Height, sweatBorderradius, MAINCOLOR); // draw sweat drop
       }

       // Sweat drop 2 -> center area
       if(sweat2YPos <= sweat2YPosMax){sweat2YPos+=0.5;} // vertical movement from initial to max
       else {sweat2XPosInitial = random((screenWidth-60))+30; sweat2YPos = 2; sweat2YPosMax = (random(10)+10); sweat2Width = 1; sweat2Height = 2;} // if max vertical position is reached: reset all values for next drop
       if(sweat2YPos <= sweat2YPosMax/2){sweat2Width+=0.5; sweat2Height+=0.5;} // shape grows in first half of animation ...
       else {sweat2Width-=0.1; sweat2Height-=0.5;} // ... and shrinks in second half of animation
       sweat2XPos = sweat2XPosInitial-(sweat2Width/2); // keep the growing shape centered to initial x position
       if (bufferEnabled) {
         drawRoundRectToBuffer(sweat2XPos, sweat2YPos, sweat2Width, sweat2Height, sweatBorderradius, MAINCOLOR); // draw sweat drop
       } else {
         display->fillRoundRect(sweat2XPos, sweat2YPos, sweat2Width, sweat2Height, sweatBorderradius, MAINCOLOR); // draw sweat drop
       }

       // Sweat drop 3 -> right corner
       if(sweat3YPos <= sweat3YPosMax){sweat3YPos+=0.5;} // vertical movement from initial to max
       else {sweat3XPosInitial = (screenWidth-30)+(random(30)); sweat3YPos = 2; sweat3YPosMax = (random(10)+10); sweat3Width = 1; sweat3Height = 2;} // if max vertical position is reached: reset all values for next drop
       if(sweat3YPos <= sweat3YPosMax/2){sweat3Width+=0.5; sweat3Height+=0.5;} // shape grows in first half of animation ...
       else {sweat3Width-=0.1; sweat3Height-=0.5;} // ... and shrinks in second half of animation
       sweat3XPos = sweat3XPosInitial-(sweat3Width/2); // keep the growing shape centered to initial x position
       if (bufferEnabled) {
         drawRoundRectToBuffer(sweat3XPos, sweat3YPos, sweat3Width, sweat3Height, sweatBorderradius, MAINCOLOR); // draw sweat drop
       } else {
         display->fillRoundRect(sweat3XPos, sweat3YPos, sweat3Width, sweat3Height, sweatBorderradius, MAINCOLOR); // draw sweat drop
       }
     }

   // Draw bounding box if enabled
   if (showBoundingBox) {
     drawBoundingBox(boundingBoxColor);
   }

   // Update tracking variables for next frame to enable selective redrawing
   prevEyeLx = eyeLx;
   prevEyeLy = eyeLy;
   prevEyeLwidth = eyeLwidthCurrent;
   prevEyeLheight = eyeLheightCurrent;
   prevEyeRx = eyeRx;
   prevEyeRy = eyeRy;
   prevEyeRwidth = eyeRwidthCurrent;
   prevEyeRheight = eyeRheightCurrent;
   prevSweat = sweat;

 } // end of drawEyes method
 
 
}; // end of class roboEyes


#endif