# FluxGarage RoboEyes for ST7735 TFT Displays

A colorful, animated robot eyes library for ST7735 TFT displays with full 16-bit RGB565 color support, frame buffering, positioning controls, and debugging features.

## Features

### 🎨 **Full Color Support**
- 16-bit RGB565 color system
- Predefined color palettes for eyes, backgrounds, and effects
- Automatic mood-based color changes
- Rainbow color cycling effects
- Custom color setting capabilities

### 👁️ **Enhanced Eye Animations**
- Smooth eye movements and blinking
- Multiple mood expressions (Happy, Angry, Tired, Sleepy, Excited, Confused, Love)
- Curious gaze effect (eyes get larger when looking to the sides)
- Cyclops mode (single eye)
- Realistic eye highlights and shadows

### 🎭 **Advanced Visual Effects**
- Sweat drops animation
- Confused and laughing animations
- Color shifting and rainbow effects
- Eyelid animations with skin tones
- Depth effects with shadows

### 📱 **Display Features**
- Optimized for 128x160 ST7735 displays
- Flexible eye positioning (anywhere on screen)
- Screen rotation support (0-3)
- High frame rate support (up to 60 FPS)
- Frame buffering for flicker-free rendering
- Efficient rendering with partial screen updates

### 🔧 **Debugging & Positioning**
- Bounding box visualization for debugging
- Eye position offset controls
- Real-time positioning feedback
- Screen dimension visualization
- Center crosshair for alignment

## Hardware Requirements

- ST7735 TFT Display (128x160 recommended)
- ESP32 or compatible microcontroller
- Wiring:
  - CS (Chip Select): GPIO 5
  - RST (Reset): GPIO 4  
  - DC (Data/Command): GPIO 2
  - MOSI, SCK: SPI pins

## Installation

1. Install the Adafruit ST7735 library:
   ```
   Arduino IDE: Tools → Manage Libraries → Search "Adafruit ST7735"
   ```

2. Copy `FluxGarage_RoboEyes-colored-st7735.h` to your project folder

3. Include the library in your sketch:
   ```cpp
   #include <Adafruit_ST7735.h>
   #include <Adafruit_GFX.h>
   #include "FluxGarage_RoboEyes-colored-st7735.h"
   ```

## Quick Start

```cpp
#include <Adafruit_ST7735.h>
#include <Adafruit_GFX.h>
#include "FluxGarage_RoboEyes-colored-st7735.h"

#define TFT_CS    5
#define TFT_RST   4
#define TFT_DC    2

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);
RoboEyes<Adafruit_ST7735> eyes = RoboEyes<Adafruit_ST7735>(tft);

void setup() {
  Serial.begin(115200);
  
  // Initialize display
  tft.initR(INITR_BLACKTAB);
  tft.setRotation(0); // Portrait mode
  
  // Initialize RoboEyes
  eyes.begin(160, 128, 60); // Full 160x128 display, 60 FPS
  
  // Enable frame buffering for smooth rendering
  eyes.enableFrameBuffer(true);
  
  // Set colors
  eyes.setDisplayColors(0x0000, 0xFFFF); // Black background, white eyes
  
  // Position eyes in bottom area
  eyes.setEyePositionOffset(0, 32);
  
  // Enable debugging bounding box
  eyes.setBoundingBox(true, 0xF800); // Red bounding box
  
  // Enable animations
  eyes.setAutoblinker(true, 3, 2);
  eyes.setIdleMode(true, 5, 3);
  eyes.setCuriosity(true);
  eyes.setMood(HAPPY);
}

void loop() {
  eyes.update();
  delay(16); // 60 FPS
}
```

## API Reference

### Initialization
```cpp
eyes.begin(int width, int height, byte frameRate)
eyes.begin(byte frameRate) // Default 128x160
eyes.setRotation(uint8_t rotation) // 0-3
eyes.enableFrameBuffer(bool enable) // Enable/disable frame buffering
```

### Color Management
```cpp
eyes.setDisplayColors(uint16_t background, uint16_t eyeColor)
eyes.setBackgroundColor(uint16_t color)
eyes.setEyeColor(uint16_t color)
eyes.setEyelidColor(uint16_t color)
eyes.setSweatColor(uint16_t color)
```

### Moods and Expressions
```cpp
eyes.setMood(mood) // DEFAULT, TIRED, ANGRY, HAPPY, SLEEPY, EXCITED, CONFUSED_MOOD, LOVE
eyes.setMoodColors(uint16_t eyeColor, uint16_t eyelidColor)
```

### Animations
```cpp
eyes.setAutoblinker(bool active, int interval, int variation)
eyes.setIdleMode(bool active, int interval, int variation)
eyes.setCuriosity(bool enable)
eyes.setCyclops(bool enable)
eyes.anim_confused()
eyes.anim_laugh()
eyes.setSweat(bool enable)
```

### Color Effects
```cpp
eyes.setRainbowEyes(bool enable, int speed)
eyes.setColorShift(bool enable, int speed)
```

### Basic Eye Control
```cpp
eyes.blink()
eyes.close()
eyes.open()
eyes.close(bool left, bool right)
eyes.open(bool left, bool right)
eyes.blink(bool left, bool right)
```

### Positioning & Debugging
```cpp
eyes.setEyePositionOffset(int xOffset, int yOffset) // Move eyes from center
eyes.setBoundingBox(bool enabled, uint16_t color) // Show/hide debug bounding box
eyes.setBoundingBoxPosition(int x, int y, int width, int height) // Set bounding box position
```

### Eye Geometry
```cpp
eyes.setWidth(byte leftEye, byte rightEye) // Set eye widths
eyes.setHeight(byte leftEye, byte rightEye) // Set eye heights
eyes.setBorderradius(byte leftEye, byte rightEye) // Set corner radius
eyes.setSpacebetween(int space) // Set space between eyes
```

## Predefined Colors

### Eye Colors
- `EYE_COLOR_BLUE` - Classic blue
- `EYE_COLOR_GREEN` - Natural green
- `EYE_COLOR_RED` - Bold red
- `EYE_COLOR_WHITE` - Pure white
- `EYE_COLOR_YELLOW` - Bright yellow
- `EYE_COLOR_CYAN` - Cyan blue
- `EYE_COLOR_MAGENTA` - Magenta
- `EYE_COLOR_ORANGE` - Orange
- `EYE_COLOR_PINK` - Pink
- `EYE_COLOR_PURPLE` - Purple

### Background Colors
- `BG_COLOR_BLACK` - Pure black
- `BG_COLOR_DARK_BLUE` - Dark blue
- `BG_COLOR_DARK_GREEN` - Dark green
- `BG_COLOR_DARK_RED` - Dark red
- `BG_COLOR_GRAY` - Medium gray
- `BG_COLOR_DARK_GRAY` - Dark gray

### Eyelid Colors
- `EYELID_COLOR_SKIN` - Natural skin tone
- `EYELID_COLOR_DARK_SKIN` - Darker skin tone
- `EYELID_COLOR_PINK` - Pink skin tone

## Examples

### Basic Example (`roboeyest7735.ino`)
- Simple setup with frame buffering
- Eye positioning in bottom area
- Bounding box debugging
- Mood cycling with smooth animations
- Serial debug output

### Positioning Examples
```cpp
// Position eyes in center (default)
eyes.setEyePositionOffset(0, 0);

// Position eyes in bottom area
eyes.setEyePositionOffset(0, 32);

// Position eyes in top area
eyes.setEyePositionOffset(0, -32);

// Position eyes to the right
eyes.setEyePositionOffset(20, 0);
```

### Debugging Examples
```cpp
// Enable red bounding box
eyes.setBoundingBox(true, 0xF800);

// Enable green bounding box
eyes.setBoundingBox(true, 0x07E0);

// Disable bounding box
eyes.setBoundingBox(false);
```

## Performance Tips

1. **Frame Rate**: Use 30-60 FPS for smooth animations
2. **Frame Buffering**: Enable for flicker-free rendering (uses more RAM)
3. **Partial Updates**: The library only updates the eye area for better performance
4. **Color Effects**: Rainbow and color shifting use more CPU - use sparingly
5. **Memory**: 16-bit colors and frame buffering use more memory
6. **Positioning**: Use `setEyePositionOffset()` for precise eye placement

## Troubleshooting

### Display Issues
- Ensure correct pin connections (CS=5, RST=4, DC=2)
- Check display initialization (`INITR_BLACKTAB` for most 1.8" displays)
- Verify SPI communication
- Use bounding box to verify screen dimensions

### Performance Issues
- Reduce frame rate if animations are choppy
- Disable frame buffering if low on RAM
- Disable unnecessary animations
- Check available RAM

### Color Issues
- Verify 16-bit color format (RGB565)
- Check color definitions
- Ensure display supports full color
- Use white eyes (0xFFFF) for maximum visibility

### Positioning Issues
- Enable bounding box to visualize eye placement
- Use `setEyePositionOffset()` for precise positioning
- Check screen dimensions (160x128)
- Verify eye area fits within screen bounds

## License

GNU General Public License v3.0

## Credits

- Original RoboEyes library by Dennis Hoelscher (FluxGarage)
- Enhanced for ST7735 with color support by AI Assistant
- Based on Adafruit GFX library

## Version History

- **v2.1.0** - Enhanced positioning and debugging
  - Frame buffering for flicker-free rendering
  - Eye positioning controls (`setEyePositionOffset`)
  - Bounding box debugging system
  - Improved rounded rectangle drawing
  - Better triangle rendering for eyelids
  - Enhanced performance optimizations
  - Serial debug output
  - Flexible screen size support (160x128)

- **v2.0.0** - Initial ST7735 color version
  - Full 16-bit color support
  - Screen rotation
  - Enhanced animations
  - Rainbow effects
  - Improved geometry for 128x160 displays
