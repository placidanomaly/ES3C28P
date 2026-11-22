#include "ES3C28P.h"
#include <Wire.h>

ES3C28P::ES3C28P() {}

ES3C28P &ES3C28P::getInstance() {
  if(instance_ == nullptr)
    instance_ = new ES3C28P();
  
  return *instance_;
}

lv_display_t *ES3C28P::getDisplay() {
  return display_;
}

lv_indev_t *ES3C28P::getTouchDev() {
  return touch_indev_;
}

void ES3C28P::begin() {
  initBacklight();
  initLcd();
  initLvglDisplay();

  initTouch();
  initLvglTouch();

  last_tick_ms_ = millis();
}

void ES3C28P::loop() {
  const uint32_t now = millis();
  lv_tick_inc(now - last_tick_ms_);
  last_tick_ms_ = now;
  lv_timer_handler();
}

void ES3C28P::initBacklight() {
  pinMode(LCD_BL_PIN, OUTPUT);
  digitalWrite(LCD_BL_PIN, HIGH);
}

void ES3C28P::initLcd() {
  tft_.init();
  applyPanelInitCommands();
  tft_.fillScreen(TFT_BLACK);
}

void ES3C28P::applyPanelInitCommands() {
  tft_.writecommand(static_cast<uint8_t>(0xCF));
  tft_.writedata(static_cast<uint8_t>(0x00));
  tft_.writedata(static_cast<uint8_t>(0xC1));
  tft_.writedata(static_cast<uint8_t>(0x30));

  tft_.writecommand(static_cast<uint8_t>(0xED));
  tft_.writedata(static_cast<uint8_t>(0x64));
  tft_.writedata(static_cast<uint8_t>(0x03));
  tft_.writedata(static_cast<uint8_t>(0x12));
  tft_.writedata(static_cast<uint8_t>(0x81));

  tft_.writecommand(static_cast<uint8_t>(0xE8));
  tft_.writedata(static_cast<uint8_t>(0x85));
  tft_.writedata(static_cast<uint8_t>(0x00));
  tft_.writedata(static_cast<uint8_t>(0x78));

  tft_.writecommand(static_cast<uint8_t>(0xCB));
  tft_.writedata(static_cast<uint8_t>(0x39));
  tft_.writedata(static_cast<uint8_t>(0x2C));
  tft_.writedata(static_cast<uint8_t>(0x00));
  tft_.writedata(static_cast<uint8_t>(0x34));
  tft_.writedata(static_cast<uint8_t>(0x02));

  tft_.writecommand(static_cast<uint8_t>(0xF7));
  tft_.writedata(static_cast<uint8_t>(0x20));

  tft_.writecommand(static_cast<uint8_t>(0xEA));
  tft_.writedata(static_cast<uint8_t>(0x00));
  tft_.writedata(static_cast<uint8_t>(0x00));

  tft_.writecommand(static_cast<uint8_t>(0xC0));
  tft_.writedata(static_cast<uint8_t>(0x13));

  tft_.writecommand(static_cast<uint8_t>(0xC1));
  tft_.writedata(static_cast<uint8_t>(0x13));

  tft_.writecommand(static_cast<uint8_t>(0xC5));
  tft_.writedata(static_cast<uint8_t>(0x22));
  tft_.writedata(static_cast<uint8_t>(0x35));

  tft_.writecommand(static_cast<uint8_t>(0xC7));
  tft_.writedata(static_cast<uint8_t>(0xBD));

  tft_.writecommand(static_cast<uint8_t>(0x21));
  tft_.writecommand(static_cast<uint8_t>(0x36));
  tft_.writedata(static_cast<uint8_t>(0x08));

  tft_.writecommand(static_cast<uint8_t>(0xB6));
  tft_.writedata(static_cast<uint8_t>(0x0A));
  tft_.writedata(static_cast<uint8_t>(0xA2));

  tft_.writecommand(static_cast<uint8_t>(0x3A));
  tft_.writedata(static_cast<uint8_t>(0x55));

  tft_.writecommand(static_cast<uint8_t>(0xF6));
  tft_.writedata(static_cast<uint8_t>(0x01));
  tft_.writedata(static_cast<uint8_t>(0x30));

  tft_.writecommand(static_cast<uint8_t>(0xB1));
  tft_.writedata(static_cast<uint8_t>(0x00));
  tft_.writedata(static_cast<uint8_t>(0x1B));

  tft_.writecommand(static_cast<uint8_t>(0xF2));
  tft_.writedata(static_cast<uint8_t>(0x00));

  tft_.writecommand(static_cast<uint8_t>(0x26));
  tft_.writedata(static_cast<uint8_t>(0x01));

  tft_.writecommand(static_cast<uint8_t>(0xE0));
  const uint8_t gammaPos[] = {
      0x0F, 0x35, 0x31, 0x0B, 0x0E, 0x06, 0x49, 0xA7,
      0x33, 0x07, 0x0F, 0x03, 0x0C, 0x0A, 0x00};
  for (uint8_t b : gammaPos) {
    tft_.writedata(b);
  }

  tft_.writecommand(static_cast<uint8_t>(0xE1));
  const uint8_t gammaNeg[] = {
      0x00, 0x0A, 0x0F, 0x04, 0x11, 0x08, 0x36, 0x58,
      0x4D, 0x07, 0x10, 0x0C, 0x32, 0x34, 0x0F};
  for (uint8_t b : gammaNeg) {
    tft_.writedata(b);
  }

  tft_.writecommand(static_cast<uint8_t>(0x11));
  delay(120);
  tft_.writecommand(static_cast<uint8_t>(0x29));
}

void ES3C28P::initLvglDisplay() {
  lv_init();

  void *buffer =
      heap_caps_malloc(DRAW_BUF_BYTES, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
  display_ = lv_display_create(DISP_HOR_RES, DISP_VER_RES);
  lv_display_set_color_format(display_, LV_COLOR_FORMAT_RGB565);
  lv_display_set_flush_cb(display_, flushCallback);
  lv_display_set_buffers(
      display_,
      buffer,
      nullptr,
      DRAW_BUF_BYTES,
      LV_DISPLAY_RENDER_MODE_PARTIAL);
}

void ES3C28P::flushCallback(lv_display_t *display,
                             const lv_area_t *area,
                             uint8_t *pixels) {
  if (instance_ == nullptr) {
    lv_display_flush_ready(display);
    return;
  }

  uint32_t width = static_cast<uint32_t>(area->x2 - area->x1 + 1);
  uint32_t height = static_cast<uint32_t>(area->y2 - area->y1 + 1);
  uint16_t *buffer16 = reinterpret_cast<uint16_t *>(pixels);

  TFT_eSPI &tft = instance_->tft_;
  tft.startWrite();
  tft.setAddrWindow(area->x1, area->y1, width, height);
  tft.pushColors(buffer16, width * height, true);
  tft.endWrite();

  lv_display_flush_ready(display);
}

void ES3C28P::initTouch() {
  pinMode(TP_INT_PIN, INPUT_PULLUP);
  pinMode(TP_RST_PIN, OUTPUT);

  digitalWrite(TP_RST_PIN, LOW);
  delay(10);
  digitalWrite(TP_RST_PIN, HIGH);
  delay(100);

  Wire.begin(TP_SDA_PIN, TP_SCL_PIN, 400000); // 400 kHz Fast Mode I2C
}

bool ES3C28P::readTouch(int32_t &x, int32_t &y) {
  Wire.beginTransmission(FT6336_ADDR);
  Wire.write(0x02);
  if (Wire.endTransmission(false) != 0) {
    return false;
  }
  if (Wire.requestFrom(FT6336_ADDR, static_cast<uint8_t>(5)) != 5) {
    return false;
  }

  uint8_t points = Wire.read() & 0x0F;
  uint8_t xh = Wire.read();
  uint8_t xl = Wire.read();
  uint8_t yh = Wire.read();
  uint8_t yl = Wire.read();

  if (points == 0) {
    return false;
  }

  x = ((xh & 0x0F) << 8) | xl;
  y = ((yh & 0x0F) << 8) | yl;
  return true;
}

void ES3C28P::touchReadCallback(lv_indev_t *indev, lv_indev_data_t *data) {
  LV_UNUSED(indev);
  int32_t x = 0;
  int32_t y = 0;

  if (instance_ != nullptr && ES3C28P::getInstance().readTouch(x, y)) {
    data->point.x = x;
    data->point.y = y;
    data->state = LV_INDEV_STATE_PRESSED;
  } else {
    data->state = LV_INDEV_STATE_RELEASED;
  }
}

void ES3C28P::initLvglTouch() {
  touch_indev_ = lv_indev_create();
  lv_indev_set_type(touch_indev_, LV_INDEV_TYPE_POINTER);
  lv_indev_set_read_cb(touch_indev_, touchReadCallback);
}
