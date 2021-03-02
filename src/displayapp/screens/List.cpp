#include "List.h"
#include "../DisplayApp.h"
#include "Symbols.h"

using namespace Pinetime::Applications::Screens;

//namespace {
  static void ButtonEventHandler(lv_obj_t * obj, lv_event_t event) {
    List* screen = static_cast<List *>(obj->user_data);
    screen->OnButtonEvent(obj, event);
  }

//}

List::List(uint8_t screenID, uint8_t numScreens,
    DisplayApp* app, 
    Controllers::DateTime& dateTimeController, 
    Controllers::Settings &settingsController, 
    std::array<Applications, MAXLISTITEMS>& applications) : 
    Screen(app),
    dateTimeController{dateTimeController},
    settingsController{settingsController},
    currentDateTime{{}}  
{

  // Set the background to Black
  lv_obj_set_style_local_bg_color(lv_scr_act(), LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_make(0, 0, 0));

  settingsController.SetSettingsMenu(screenID);
  /*uint8_t hours = dateTimeController.Hours();
  uint8_t minutes = dateTimeController.Minutes();
  oldHours = hours;
  oldMinutes = minutes;*/

  // Time
  /*label_time = lv_label_create(lv_scr_act(), nullptr);  
  lv_label_set_text_fmt(label_time,  "%02i:%02i", hours, minutes);      
  lv_label_set_align( label_time, LV_LABEL_ALIGN_CENTER );    
  lv_obj_align(label_time, lv_scr_act(), LV_ALIGN_IN_TOP_MID, 0, 4);*/

  if ( numScreens > 1 ) {
    pageIndicatorBasePoints[0].x = 240 - 3;
    pageIndicatorBasePoints[0].y = 6;
    pageIndicatorBasePoints[1].x = 240 - 3;
    pageIndicatorBasePoints[1].y = 240 - 6;
    
    pageIndicatorBase = lv_line_create(lv_scr_act(), NULL);
    lv_obj_set_style_local_line_width(pageIndicatorBase, LV_LINE_PART_MAIN, LV_STATE_DEFAULT, 6);
    lv_obj_set_style_local_line_color(pageIndicatorBase, LV_LINE_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x111111));
    lv_obj_set_style_local_line_rounded(pageIndicatorBase, LV_LINE_PART_MAIN, LV_STATE_DEFAULT, true);
    lv_line_set_points(pageIndicatorBase, pageIndicatorBasePoints, 2);

  
    uint16_t indicatorSize = 228 / numScreens;
    uint16_t indicatorPos = indicatorSize * screenID;

    pageIndicatorPoints[0].x = 240 - 3;
    pageIndicatorPoints[0].y = 6 + indicatorPos;
    pageIndicatorPoints[1].x = 240 - 3;
    pageIndicatorPoints[1].y = 6 + indicatorPos + indicatorSize;

    pageIndicator = lv_line_create(lv_scr_act(), NULL);
    lv_obj_set_style_local_line_width(pageIndicator, LV_LINE_PART_MAIN, LV_STATE_DEFAULT, 6);
    lv_obj_set_style_local_line_color(pageIndicator, LV_LINE_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_CYAN);
    lv_obj_set_style_local_line_rounded(pageIndicator, LV_LINE_PART_MAIN, LV_STATE_DEFAULT, true);
    lv_line_set_points(pageIndicator, pageIndicatorPoints, 2);
  }


  lv_obj_t* container1 = lv_cont_create(lv_scr_act(), nullptr);

  //lv_obj_set_style_local_bg_color(container1, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x111111));
  lv_obj_set_style_local_bg_opa(container1, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_TRANSP);
  lv_obj_set_style_local_pad_all(container1, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, 10);
  lv_obj_set_style_local_pad_inner(container1, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, 5);
  lv_obj_set_style_local_border_width(container1, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, 0);

  lv_obj_set_pos(container1, 0, 0);
  lv_obj_set_width(container1, LV_HOR_RES - 15);
  lv_obj_set_height(container1, LV_VER_RES);
  //lv_obj_set_auto_realign(container1, true);                  
  //lv_obj_align_origo(container1, NULL, LV_ALIGN_CENTER, 0, 0);
  //lv_cont_set_fit(container1, LV_FIT_MAX);
  lv_cont_set_layout(container1, LV_LAYOUT_COLUMN_LEFT);

  lv_obj_t * labelBt;
  lv_obj_t * labelBtIco;

  for(int i = 0; i < MAXLISTITEMS; i++) {
    apps[i] = applications[i].application;
    if ( applications[i].application != Apps::None) {    

      itemApps[i] = lv_btn_create(container1, nullptr);
      lv_obj_set_style_local_bg_opa(itemApps[i], LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_30);
      //lv_obj_set_style_local_bg_color(itemApps[i], LV_BTN_PART_MAIN, LV_STATE_PRESSED, LV_COLOR_CYAN);
      lv_obj_set_width(itemApps[i], LV_HOR_RES - 25);
      lv_obj_set_height(itemApps[i], 52);
      lv_obj_set_event_cb(itemApps[i], ButtonEventHandler);
      lv_btn_set_layout(itemApps[i], LV_LAYOUT_ROW_MID);
      itemApps[i]->user_data = this;

      labelBtIco = lv_label_create(itemApps[i], nullptr);
      lv_obj_set_style_local_text_color(labelBtIco, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_YELLOW);
      lv_obj_set_style_local_text_font(labelBtIco, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &lv_font_sys_20);  
      lv_label_set_text_static(labelBtIco, applications[i].icon);

      labelBt = lv_label_create(itemApps[i], nullptr);
      lv_label_set_text_fmt(labelBt, " %s", applications[i].name);
      
    }
  }

  lv_obj_t * backgroundLabel = lv_label_create(lv_scr_act(), nullptr);
  lv_label_set_long_mode(backgroundLabel, LV_LABEL_LONG_CROP);
  lv_obj_set_size(backgroundLabel, LV_HOR_RES, LV_VER_RES);
  lv_obj_set_pos(backgroundLabel, 0, 0);
  lv_label_set_text_static(backgroundLabel, "");

}

List::~List() {  
  lv_obj_clean(lv_scr_act());
}

bool List::Refresh() {

  return running;
}

void List::OnButtonEvent(lv_obj_t * object, lv_event_t event) {
  if ( event == LV_EVENT_RELEASED ) {
    for(int i = 0; i < MAXLISTITEMS; i++) {
      if ( apps[i] != Apps::None && object == itemApps[i] ) {
        app->StartApp(apps[i], DisplayApp::FullRefreshDirections::None);
        running = false;
        return;
      }
    }
  }
}

