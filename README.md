# ES3C28P

ES3C28P is a compact ESP32-S3-based development board sold on AliExpress featuring a 2.8" SPI LCD display (ILI9341) and a capacitive touchscreen (FT6336).

![ES3C28P](./assets/ES3C28P.png)

The vendor-supplied documentation for this board is cryptic and suggests dropping replacement files into standard Arduino libraries, which should be avoided. This library provides a clean alternative without modifying any third-party library files. It configures the ILI9341 LCD with `TFT_eSPI`, initializes **LVGL 9.6**, and registers the FT6336 touchscreen as an LVGL input device.

No special helper functions are needed to access the exposed UART or I2C pins on the back of the board—you can use standard Arduino `Serial` and `Wire` libraries directly.

> **Note on Screen Orientation:** This library was tested in portrait orientation with the USB port at the bottom (short LCD edge horizontal). Using the other three orientations will require adjustments to map touch coordinates to screen coordinates. Orientation support is planned for an upcoming release.

The board also includes a speaker output, battery input, and microphone input. While there are no immediate plans to implement drivers for these peripherals, pull requests to add them are welcome.

## Arduino IDE Settings

Compiling sketches for this board requires the following settings under the **Tools** menu in Arduino IDE (2.x recommended):

- **Board:** ESP32S3 Dev Module
- **USB CDC On Boot:** Enabled
- **CPU Frequency:** 240MHz (WiFi)
- **Flash Mode:** DIO 80MHz
- **Flash Size:** 16MB (128Mb)
- **Partition Scheme:** Huge APP (3MB No OTA/1MB SPIFFS)
- **Upload Mode:** UART0 / Hardware CDC
- **Upload Speed:** 921600
- **USB Mode:** Hardware CDC and JTAG

## Usage

### Arduino IDE

1. Open the Arduino IDE **Library Manager**, search for **ES3C28P**, and install it (along with its dependencies, `TFT_eSPI` and `lvgl`).
2. Copy the `build_opt.h` file from this library's `examples/basic` folder into the same directory as your `.ino` sketch file.

Arduino IDE 2.x uses `build_opt.h` to pass compiler flags directly to `TFT_eSPI` and `LVGL` without editing library source files. It must contain the following directives (see comments in `es3c28p.h` for details):

```text
-DUSER_SETUP_LOADED=1
-DLV_CONF_SKIP
-DILI9341_DRIVER=1
-DTFT_WIDTH=240
-DTFT_HEIGHT=320
-DTFT_MISO=13
-DTFT_MOSI=11
-DTFT_SCLK=12
-DTFT_CS=10
-DTFT_DC=46
-DTFT_RST=-1
-DTFT_BL=45
-DTFT_BACKLIGHT_ON=HIGH
-DTFT_INVERSION_ON=1
-DUSE_HSPI_PORT=1
-DSPI_FREQUENCY=27000000
-DSPI_READ_FREQUENCY=16000000
```

### PlatformIO

If you are using PlatformIO, copy the `platformio.ini` from `examples_platformio/basic` into your project. It includes all required compiler flags under `build_flags` (since PlatformIO does not read Arduino IDE's `build_opt.h`) and pulls `ES3C28P` along with its `TFT_eSPI` and `LVGL` dependencies automatically via `lib_deps`.

### Minimal Arduino Sketch and PlatformIO program

```cpp
#include <ES3C28P.h>

ES3C28P *board = nullptr;

void setup() {
  board = &ES3C28P::getInstance();

  Serial.begin(115200);
  delay(1000);

  board->begin();
  Serial.println("Board initialized");
}

void loop() {
  board->loop();
  vTaskDelay(pdMS_TO_TICKS(5));
}
```

## Examples

Both examples below display a SquareLine Studio UI with a checkbox, draw an LVGL object, and handle drag and click touch events:

- **Arduino IDE (`examples/basic`):** Open `basic.ino` in the Arduino IDE and click **Upload**. *(Note: The initial compilation will take several minutes while `LVGL` and `TFT_eSPI` compile).*
- **PlatformIO (`examples_platformio/basic`):** Open the `examples_platformio/basic` folder in VS Code with the PlatformIO extension. The included `platformio.ini` is pre-configured with all required build flags and libraries.

The SquareLine Studio design is the `.spj` file in the `assets` folder. It can be modified and exported into the `ui` folder of the example programs.

## Future Improvements

Contributions and pull requests are welcome for the following features:

- Support for all four screen orientations (including automatic touch coordinate rotation)
- Full initialization and control of all onboard peripherals
- LCD power ON/OFF and hardware reset controls
- Backlight PWM brightness control
- Light and Deep Sleep power management
- Speaker (I2S) and microphone support

## Contact Information

For questions, bug reports, or contributions, please open an issue on GitHub or contact [Charles Vaillancourt](mailto:charles.vaillancourt@gmail.com).
