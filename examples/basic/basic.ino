#include <Arduino.h>
#include <es3c28p.h>
#include "src/ui/ui.h"

ES3C28P *board = nullptr;
 
// Test UI variables
lv_obj_t *touch_circle_ = nullptr;
lv_obj_t *label_ = nullptr;
lv_point_t drag_offset_ = {};
static constexpr int32_t CIRCLE_SIZE = 24;

static void onCheckboxChanged(lv_event_t *event) {
  lv_obj_t *checkbox = lv_event_get_target_obj(event);
  const bool checked = lv_obj_has_state(checkbox, LV_STATE_CHECKED);
  Serial.printf("Checkbox %s\n", checked ? "checked" : "unchecked");
}

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

  label_ = lv_label_create(screen);
  lv_label_set_text(label_, "LVGL 9.6 on ESP32-S3");
  lv_obj_set_style_text_color(label_, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
  lv_obj_center(label_);
  lv_obj_set_event_bubble(label_, true);

  touch_circle_ = lv_obj_create(screen);
  lv_obj_set_size(touch_circle_, CIRCLE_SIZE, CIRCLE_SIZE);
  lv_obj_set_style_radius(touch_circle_, LV_RADIUS_CIRCLE, LV_PART_MAIN);
  lv_obj_set_style_bg_color(
      touch_circle_, lv_color_hex(0xFFE600), LV_PART_MAIN);
  lv_obj_set_style_border_color(
      touch_circle_, lv_color_hex(0xFF2A2A), LV_PART_MAIN);
  lv_obj_set_style_border_width(touch_circle_, 2, LV_PART_MAIN);
  lv_obj_set_pos(touch_circle_, 10, 10);
  lv_obj_set_event_bubble(touch_circle_, true);
  lv_obj_add_event_cb(
      touch_circle_, dragCircleCallback, LV_EVENT_PRESSED, nullptr);
  lv_obj_add_event_cb(
      touch_circle_, dragCircleCallback, LV_EVENT_PRESSING, nullptr);
  lv_obj_add_event_cb(
      screen, screenTouchCallback, LV_EVENT_PRESSED, nullptr);
}

void setup() {
  board = &ES3C28P::getInstance();

  Serial.begin(115200);
  delay(1000);
  Serial.println("In setup");

  board->begin();
  Serial.println("Board initialized");

  ui_init();
  Serial.println("Squareline Studios UI initialized");

  lv_obj_add_event_cb(
      ui_checkbox1, onCheckboxChanged, LV_EVENT_VALUE_CHANGED, nullptr);
  createTestUi();
  Serial.println("Test UI initialized. Setup complete.");
}

void loop() {
  board->loop();
  vTaskDelay(pdMS_TO_TICKS(5));
}
