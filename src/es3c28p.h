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

#ifndef TFT_WIDTH
  #define TFT_WIDTH 240
#endif

#ifndef TFT_HEIGHT
  #define TFT_HEIGHT 320
#endif

class ES3C28P {
  public:
    /**
     * @brief Returns a reference to a singleton of the ES3C28P
     * @details Because there can only be a single ES3C28P board in any
     * given program, the class has to be a singleton. This method
     * takes care of creating the first instance of the ES3C28P object and,
     * if it already exists, returns a reference to it.
     * @return A reference to the singleton ES3C28P object (which can be
     * dereferenced to a pointer with &)
    */
    static ES3C28P &getInstance();
    
    /**
     * @brief MUST be called by the main program's setup() function
     * @details Initializes the LCD driver chip, the LCD display, the touch 
     * controller, and connects all of those to the LVGL framework.
     */
    void begin();

    /**
     * @brief MUST be called by the main program's loop() function
     * @details If not called, the entire LVGL framework, including rendering,
     * touchscreen events, etc., will not work.
     */
    void loop();

    /**
     * @brief Returns a pointer to the low-level LVGL display object
     * @returns A lv_display_t * pointer to the LVGL display 
     */
    lv_display_t *getDisplay();
    
    /**
     * @brief Returns a pointer to the LVGL touch device
     * @returns A lv_indev_t * pointer to the LVGL touch input device
     */
    lv_indev_t *getTouchDev();


    /**
     * @brief Sets the rotation of the LCD display
     * @details  Whenever called, the UI must be recreated. For Squareline 
     * Studio-generated UIs, this means calling ui_init() after setRotation().
     * @param[in] rotation A value of 0 sets the orientation to portrait mode (i.e.
     * short side of LCD horizontal) with the USB type C connector pointing
     * down. Higher values of 1, 2, and 3, each rotate the screen 90 degrees
     * counterclockwise relative to the previous value.
     */
    void setRotation(uint8_t rotation);

    /**
     * @brief Returns the current rotation setting of the display
     * @returns The value corresponding to the current orientation of the
     * display. See setRotation() for the meaning of possible return values.
     */
    uint8_t getRotation() const { return rotation_; }

    /**
     * @brief Sets the brightness of the LCD backlight
     * @param[in] brightness A value of 0 (backlight turned OFF) to 255
     * (100% intensity) 
     */
    void setBrightness(uint8_t brightness); // 0 (off) to 255 (full brightness)

    /**
     * @brief Returns the current backlight brightness setting
     * @returns A value between 0 and 255 corresponding to the current brightness
     * of the LCD backlight
     */
    uint8_t getBrightness() const { return brightness_; }

    static constexpr uint32_t LCD_BL_PIN = 45;
    static constexpr uint32_t DISP_HOR_RES = 240;
    static constexpr uint32_t DISP_VER_RES = 320;

  private:
    ES3C28P();

    static constexpr uint32_t BUF_LINES = 20;
    static constexpr size_t DRAW_BUF_BYTES =
        DISP_HOR_RES * BUF_LINES * sizeof(lv_color_t);

    static constexpr int TP_SDA_PIN = 16;
    static constexpr int TP_SCL_PIN = 15;
    static constexpr int TP_RST_PIN = 18;
    static constexpr int TP_INT_PIN = 17;
    static constexpr uint8_t FT6336_ADDR = 0x38;
    static constexpr uint32_t LCD_BL_PWM_FREQ = 5000;
    static constexpr uint8_t  LCD_BL_PWM_RES  = 8; // 8-bit (0-255)
    static constexpr uint8_t  LCD_BL_PWM_CH   = 0; // Used only on Core 2.x

    TFT_eSPI tft_;
    lv_display_t *display_ = nullptr;
    lv_indev_t *touch_indev_ = nullptr;
    uint32_t last_tick_ms_ = 0;
    uint8_t rotation_ = 0;
    uint8_t brightness_ = 255;

    inline static ES3C28P *instance_ = nullptr;

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
