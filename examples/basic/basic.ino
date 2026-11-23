#include <ES3C28P.h>
#include "src/ui/ui.h"

ES3C28P *board = nullptr;
 
// Test UI variables
lv_obj_t *touch_circle_ = nullptr;
lv_point_t drag_offset_ = {};
static constexpr int32_t CIRCLE_SIZE = 24;


void screenTouchCallback(lv_event_t *event) {
  if (touch_circle_ == nullptr ||
      lv_event_get_target(event) == touch_circle_) {
    return;
  }

  lv_indev_t *indev = lv_indev_active();
  if (indev == nullptr) {
    return;
  }

  lv_point_t point;
  lv_indev_get_point(indev, &point);

  int32_t x = point.x - (CIRCLE_SIZE / 2);
  int32_t y = point.y - (CIRCLE_SIZE / 2);
  const int32_t max_x =
      lv_display_get_horizontal_resolution(board->getDisplay()) -
      lv_obj_get_width(touch_circle_);
  const int32_t max_y =
      lv_display_get_vertical_resolution(board->getDisplay()) -
      lv_obj_get_height(touch_circle_);

  if (x < 0) x = 0;
  if (y < 0) y = 0;
  if (x > max_x) x = max_x;
  if (y > max_y) y = max_y;

  lv_obj_set_pos(touch_circle_, x, y);
}

void dragCircleCallback(lv_event_t *event) {
  
  if (touch_circle_ == nullptr) {
    return;
  }

  lv_indev_t *indev = lv_indev_active();
  if (indev == nullptr) {
    return;
  }

  lv_point_t point;
  lv_indev_get_point(indev, &point);

  if (lv_event_get_code(event) == LV_EVENT_PRESSED) {
    drag_offset_.x = point.x - lv_obj_get_x(touch_circle_);
    drag_offset_.y = point.y - lv_obj_get_y(touch_circle_);
    return;
  }

  int32_t x = point.x - drag_offset_.x;
  int32_t y = point.y - drag_offset_.y;
  const int32_t max_x =
      lv_display_get_horizontal_resolution(board->getDisplay()) -
      lv_obj_get_width(touch_circle_);
  const int32_t max_y =
      lv_display_get_vertical_resolution(board->getDisplay()) -
      lv_obj_get_height(touch_circle_);

  if (x < 0) x = 0;
  if (y < 0) y = 0;
  if (x > max_x) x = max_x;
  if (y > max_y) y = max_y;

  lv_obj_set_pos(touch_circle_, x, y);
}

void createTestUi() {
  lv_obj_t *screen = lv_screen_active();
  lv_obj_set_style_bg_color(screen, lv_color_black(), LV_PART_MAIN);

  touch_circle_ = lv_obj_create(screen);
  lv_obj_remove_style_all(touch_circle_); // Clears ALL theme styles, scrollbars, padding, and shadows
  lv_obj_set_size(touch_circle_, CIRCLE_SIZE, CIRCLE_SIZE);
  lv_obj_set_pos(touch_circle_, 10, 10);
  lv_obj_set_style_radius(touch_circle_, LV_RADIUS_CIRCLE, 0);
  lv_obj_set_style_bg_color(touch_circle_, lv_color_hex(0xFFE600), 0);
  lv_obj_set_style_bg_opa(touch_circle_, LV_OPA_COVER, 0);
  lv_obj_set_style_border_color(touch_circle_, lv_color_hex(0xFF2A2A), 0);
  lv_obj_set_style_border_width(touch_circle_, 2, 0);

  lv_obj_set_pos(touch_circle_, 10, 50);
  lv_obj_set_event_bubble(touch_circle_, true);
  lv_obj_add_event_cb(
      touch_circle_, dragCircleCallback, LV_EVENT_PRESSED, nullptr);
  lv_obj_add_event_cb(
      touch_circle_, dragCircleCallback, LV_EVENT_PRESSING, nullptr);
  lv_obj_add_event_cb(
      screen, screenTouchCallback, LV_EVENT_PRESSED, nullptr);
}

extern "C" void onBrightnessSliderChanged(lv_event_t * e) {
  ES3C28P *board = &ES3C28P::getInstance();
  
  board->setBrightness(lv_slider_get_value(ui_brightnessSlider));
}

extern "C" void onScreenLongPress(lv_event_t * e) {
  ES3C28P *board = &ES3C28P::getInstance();
  
  board->setBrightness(255);
  lv_slider_set_value(ui_brightnessSlider, 255, LV_ANIM_ON);
}

extern "C" void onRotateButtonClicked(lv_event_t * e) {
  ES3C28P *board = &ES3C28P::getInstance();

  board->setRotation(board->getRotation() + 1);
  ui_init();
  createTestUi();
}

void setup() {
  board = &ES3C28P::getInstance();

  Serial.begin(115200);
  delay(1000);
  Serial.println("In setup");

  board->begin();
  board->setRotation(0);
  board->setBrightness(255); 
  Serial.println("Board initialized");

  ui_init();
  Serial.println("Squareline Studios UI initialized");

  createTestUi();
  Serial.println("Test UI initialized. Setup complete.");
}

void loop() {
  board->loop();
  vTaskDelay(pdMS_TO_TICKS(5));
}
