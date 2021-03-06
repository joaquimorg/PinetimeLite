#include "Steps.h"
#include <lvgl/lvgl.h>
#include "../DisplayApp.h"
#include "board_config.h"

//LV_IMG_DECLARE(icon_running);

using namespace Pinetime::Applications::Screens;


static void lv_update_task(struct _lv_task_t *task) {  
  auto user_data = static_cast<Steps *>(task->user_data);
  user_data->UpdateScreen();
}

Steps::Steps(
    Pinetime::Applications::DisplayApp *app, 
    Pinetime::Controllers::Accelerometer& accelerometer,
    Controllers::Settings &settingsController) 
      : Screen(app), 
      accelerometer{accelerometer},
      settingsController{settingsController} {

  accelerometer.Update();

  stepsArc = lv_arc_create(lv_scr_act(), nullptr);

  lv_obj_set_style_local_bg_opa(stepsArc, LV_ARC_PART_BG, LV_STATE_DEFAULT, LV_OPA_0);  
  lv_obj_set_style_local_border_width(stepsArc, LV_ARC_PART_BG, LV_STATE_DEFAULT, 0);  
  lv_obj_set_style_local_radius(stepsArc, LV_ARC_PART_BG, LV_STATE_DEFAULT, 0);  
  lv_obj_set_style_local_line_color(stepsArc, LV_ARC_PART_INDIC, LV_STATE_DEFAULT, lv_color_hex(0x0000FF));  
  lv_arc_set_end_angle(stepsArc, 200);
  lv_obj_set_size(stepsArc, 220, 220);
  lv_arc_set_range(stepsArc, 0, 500);
  lv_obj_align(stepsArc, nullptr, LV_ALIGN_CENTER, 0, 0);

  steps_icon = lv_img_create(lv_scr_act(), nullptr);
  lv_img_set_src(steps_icon, "F:/walk1.bin");
  lv_obj_align(steps_icon, nullptr, LV_ALIGN_CENTER, 0, -25);

  lSteps = lv_label_create(lv_scr_act(), nullptr);
  lv_obj_set_style_local_text_color(lSteps, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x00FF00));
  lv_obj_set_style_local_text_font(lSteps, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &lv_font_clock_42);   
  lv_label_set_text_fmt(lSteps,"%li", accelerometer.GetSteps()); 
  lv_obj_align(lSteps, nullptr, LV_ALIGN_CENTER, 0, 40);

  lv_arc_set_value(stepsArc, int16_t(500 * accelerometer.GetSteps() / settingsController.GetStepsGoal()));

  lv_obj_t * lstepsL = lv_label_create(lv_scr_act(), nullptr);  
  lv_label_set_text_fmt(lstepsL,"Steps\n%i", settingsController.GetStepsGoal()); 
  lv_label_set_align(lstepsL, LV_LABEL_ALIGN_CENTER);
  lv_obj_align(lstepsL, lSteps, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);

  lv_obj_t * backgroundLabel = lv_label_create(lv_scr_act(), nullptr);
  lv_label_set_long_mode(backgroundLabel, LV_LABEL_LONG_CROP);
  lv_obj_set_size(backgroundLabel, 240, 240);
  lv_obj_set_pos(backgroundLabel, 0, 0);
  lv_label_set_text_static(backgroundLabel, "");

  taskUpdate = lv_task_create(lv_update_task, 5000, LV_TASK_PRIO_MID, this);

}

Steps::~Steps() {
  lv_task_del(taskUpdate);
  lv_obj_clean(lv_scr_act());
}

void Steps::UpdateScreen() {
    currentImage++;
    if(currentImage > 4) currentImage = 1;
    switch (currentImage) {
      case 1:
          lv_img_set_src(steps_icon, "F:/walk1.bin");
          break;
      case 2:
          lv_img_set_src(steps_icon, "F:/walk2.bin");
          break;
      case 3:
          lv_img_set_src(steps_icon, "F:/walk3.bin");
          break;
      case 4:
          lv_img_set_src(steps_icon, "F:/walk4.bin");
          break;
      default:
          break;
    }
}

bool Steps::Refresh() {
  
  accelerometer.Update();

  lv_label_set_text_fmt(lSteps,"%li", accelerometer.GetSteps());
  lv_obj_align(lSteps, nullptr, LV_ALIGN_CENTER, 0, 40);

  stepCount = accelerometer.GetSteps();
  if(stepCount.IsUpdated()) {        
    lv_arc_set_value(stepsArc, int16_t(500 * stepCount.Get() / settingsController.GetStepsGoal()));
  }  

  return running;
}


