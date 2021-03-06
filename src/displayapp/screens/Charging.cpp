#include "Charging.h"
#include "../DisplayApp.h"
#include "board_config.h"
#include "components/battery/BatteryController.h"

using namespace Pinetime::Applications::Screens;

//LV_IMG_DECLARE(icon_charging);

static void lv_update_task(struct _lv_task_t *task) {  
  auto user_data = static_cast<Charging *>(task->user_data);
  user_data->UpdateScreen();
}

static void lv_anim_task(struct _lv_task_t *task) {  
  auto user_data = static_cast<Charging *>(task->user_data);
  user_data->UpdateAnim();
}



Charging::Charging(
    Pinetime::Applications::DisplayApp *app, 
    Pinetime::Controllers::Battery& batteryController) : 
  Screen(app),
  batteryController{batteryController}
{

  batteryPercent = batteryController.PercentRemaining();
  batteryVoltage = batteryController.Voltage();

  lv_obj_t * charging_ico = lv_img_create(lv_scr_act(), NULL);
  lv_img_set_src(charging_ico, "F:/icon_charging.bin");
  lv_obj_align(charging_ico, NULL, LV_ALIGN_CENTER, -35, -55);

  charging_bar = lv_bar_create(lv_scr_act(), NULL);
  lv_obj_set_size(charging_bar, 200, 15);
  lv_bar_set_range(charging_bar, 0, 100);
  lv_obj_align(charging_bar, NULL, LV_ALIGN_CENTER, 0, 10);
  lv_bar_set_anim_time(charging_bar, 2000);
  lv_obj_set_style_local_radius(charging_bar, LV_BAR_PART_BG, LV_STATE_DEFAULT, LV_RADIUS_CIRCLE);
  lv_obj_set_style_local_bg_color(charging_bar, LV_BAR_PART_BG, LV_STATE_DEFAULT, lv_color_hex(0x222222));
  lv_obj_set_style_local_bg_opa(charging_bar, LV_BAR_PART_BG, LV_STATE_DEFAULT, LV_OPA_100);
  lv_obj_set_style_local_bg_color(charging_bar, LV_BAR_PART_INDIC , LV_STATE_DEFAULT, lv_color_hex(0xFF0000));
  lv_bar_set_value(charging_bar, batteryPercent, LV_ANIM_OFF);

  status = lv_label_create(lv_scr_act(), NULL);  
  lv_label_set_text_static(status,"Reading Battery status");
  lv_label_set_align(status, LV_LABEL_ALIGN_CENTER);
  lv_obj_align(status, charging_bar, LV_ALIGN_OUT_BOTTOM_MID, 0, 20);
  
  percent = lv_label_create(lv_scr_act(), NULL);
  lv_obj_set_style_local_text_font(percent, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &lv_font_clock_42);
  if ( batteryPercent >= 0) {
    lv_label_set_text_fmt(percent,"%i%%", batteryPercent);
  } else {
    lv_label_set_text(percent,"--%");
  }
  lv_label_set_align(percent, LV_LABEL_ALIGN_LEFT);
  lv_obj_align(percent, charging_ico, LV_ALIGN_OUT_RIGHT_MID, 15, 0);

  voltage = lv_label_create(lv_scr_act(), NULL);  
  lv_obj_set_style_local_text_color(voltage, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xC6A600));
  lv_label_set_text_fmt(voltage,"%.2f volts", batteryVoltage);
  lv_label_set_align(voltage, LV_LABEL_ALIGN_CENTER);
  lv_obj_align(voltage, NULL, LV_ALIGN_CENTER, 0, 95);

  lv_obj_t * backgroundLabel = lv_label_create(lv_scr_act(), nullptr);
  lv_label_set_long_mode(backgroundLabel, LV_LABEL_LONG_CROP);
  lv_obj_set_size(backgroundLabel, 240, 240);
  lv_obj_set_pos(backgroundLabel, 0, 0);
  lv_label_set_text_static(backgroundLabel, "");

  taskUpdate = lv_task_create(lv_update_task, 60000, LV_TASK_PRIO_LOW, this);
  taskAnim = lv_task_create(lv_anim_task, 1000, LV_TASK_PRIO_LOW, this);
  UpdateScreen();
}


Charging::~Charging() {
  lv_task_del(taskUpdate);
  lv_task_del(taskAnim);
  lv_obj_clean(lv_scr_act());
}

void Charging::UpdateAnim() {
  batteryPercent = batteryController.PercentRemaining();

  if ( batteryPercent >= 0 ) {
    if ( batteryController.IsCharging() and batteryPercent < 100 ) {
      animation +=1;
      if (animation >= 100) {
        animation = 0;
      }

    } else {
      if (animation > batteryPercent) {
        animation--;
      }
      if (animation < batteryPercent) {
        animation++;
      }
    }

    lv_bar_set_value(charging_bar, animation, LV_ANIM_OFF);
  }
}

void Charging::UpdateScreen() {

  batteryController.Update();

  batteryPercent = batteryController.PercentRemaining();
  batteryVoltage = batteryController.Voltage();

  if ( batteryPercent >= 0 ) {
    if ( batteryController.IsCharging() and batteryPercent < 100 ) {
      lv_obj_set_style_local_bg_color(charging_bar, LV_BAR_PART_INDIC , LV_STATE_DEFAULT, LV_COLOR_RED);
      lv_label_set_text_static(status,"Battery charging");
    } else if ( !batteryController.IsCharging() and batteryController.IsPowerPresent() ) {
      lv_obj_set_style_local_bg_color(charging_bar, LV_BAR_PART_INDIC , LV_STATE_DEFAULT, LV_COLOR_BLUE);
      lv_label_set_text_static(status,"Battery is fully charged");
    } else if ( batteryPercent < 10 ) {
      lv_obj_set_style_local_bg_color(charging_bar, LV_BAR_PART_INDIC , LV_STATE_DEFAULT, LV_COLOR_YELLOW);
      lv_label_set_text_static(status,"Battery is low");
    } else {
      lv_obj_set_style_local_bg_color(charging_bar, LV_BAR_PART_INDIC , LV_STATE_DEFAULT, LV_COLOR_GREEN);
      lv_label_set_text_static(status,"Battery discharging");
    }
    
    lv_label_set_text_fmt(percent,"%i%%", batteryPercent);
    
  } else {
    lv_label_set_text_static(status,"Reading Battery status");
    lv_label_set_text(percent,"--%");
  }

  lv_obj_align(status, charging_bar, LV_ALIGN_OUT_BOTTOM_MID, 0, 20);
  lv_label_set_text_fmt(voltage,"%.2f volts", batteryVoltage);
 
}

bool Charging::Refresh() {
  
  return running;
}

