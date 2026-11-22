#pragma once

// The TFT_eSPI library relies on preprocessor flags at compile time to match
// the display hardware on the ES3C28P.
//
// PlatformIO:
//   Add the build_flags and lib_deps below to your platformio.ini file.
//
// Arduino IDE (2.x or newer):
//   1. Copy build_opt.h from one of this library's example folders into
//      your sketch folder (the same folder as your .ino file).
//   2. Set Tools > PSRAM to "OPI PSRAM".
//   3. Set Tools > USB CDC On Boot to "Enabled" (for Type-C Serial output).
/*
build_flags =
  ; USB Type-C Serial support
  -D ARDUINO_USB_MODE=1
  -D ARDUINO_USB_CDC_ON_BOOT=1
  ; We're fine with the default lv_conf.h from LVGL 9.6. REMOVE THIS LINE
  ; if you want to customize LVGL by including a custom lv_conf.h header
  ; in the compilation
  -D LV_CONF_SKIP
  ; TFT_eSPI hardware configuration
  -D USER_SETUP_LOADED=1
  -D ILI9341_DRIVER=1
  -D TFT_WIDTH=240
  -D TFT_HEIGHT=320
  -D TFT_MISO=13
  -D TFT_MOSI=11
  -D TFT_SCLK=12
  -D TFT_CS=10
  -D TFT_DC=46
  -D TFT_RST=-1
  -D TFT_BL=45
  -D TFT_BACKLIGHT_ON=HIGH
  -D TFT_INVERSION_ON=1
  -D USE_HSPI_PORT=1
  -D SPI_FREQUENCY=27000000
  -D SPI_READ_FREQUENCY=16000000
  ; Optional: uncomment if drawing text directly via TFT_eSPI instead of LVGL
  ; -D LOAD_GLCD=1
  ; -D LOAD_FONT2=1

lib_deps =
  bodmer/TFT_eSPI
  lvgl/lvgl @ ^9.6.0
*/

#if !defined(USER_SETUP_LOADED)
  #ifdef PLATFORMIO
    #error "ES3C28P requires TFT_eSPI build flags. Add the required build_flags to platformio.ini (PlatformIO). See esp32_2.8.h for details."
  #else
    #error "ES3C28P requires TFT_eSPI build flags. Copy build_opt.h from one of the library's example programs to your sketch folder."
  #endif
#endif

#include <Arduino.h>

// We suppress a warning from TFT_eSPI about a touch variable
// not being defined because we do not use TFT_eSPI for touch,
// we use an I2C connection to the FT6336 touch module which
// is then wired into LVGL
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcpp"
#include <TFT_eSPI.h>
#pragma GCC diagnostic pop

#include <lvgl.h>


class Esp32_28 {
  public:
    static Esp32_28 &getInstance();
    
    void begin();
    void loop();

    lv_display_t *getDisplay();
    lv_indev_t *getTouchDev();

    static constexpr uint32_t LCD_BL_PIN = 45;
    static constexpr uint32_t DISP_HOR_RES = 240;
    static constexpr uint32_t DISP_VER_RES = 320;

  private:
    Esp32_28();

    static constexpr uint32_t BUF_LINES = 20;
    static constexpr size_t DRAW_BUF_BYTES =
        DISP_HOR_RES * BUF_LINES * sizeof(lv_color_t);

    static constexpr int TP_SDA_PIN = 16;
    static constexpr int TP_SCL_PIN = 15;
    static constexpr int TP_RST_PIN = 18;
    static constexpr int TP_INT_PIN = 17;
    static constexpr uint8_t FT6336_ADDR = 0x38;

    TFT_eSPI tft_;
    lv_display_t *display_ = nullptr;
    lv_indev_t *touch_indev_ = nullptr;
    uint32_t last_tick_ms_ = 0;

    inline static Esp32_28 *instance_ = nullptr;

    void initBacklight();
    void initLcd();
    void applyPanelInitCommands();
    void initLvglDisplay();
    void initTouch();
    void initLvglTouch();
    bool readTouch(int32_t &x, int32_t &y);

    static void flushCallback(lv_display_t *display,
                              const lv_area_t *area,
                              uint8_t *pixels);
    static void touchReadCallback(lv_indev_t *indev, lv_indev_data_t *data);
};
