#include "Notifications.h"
#include <displayapp/DisplayApp.h>

using namespace Pinetime::Applications::Screens;

/*
LV_IMG_DECLARE(not_email);
LV_IMG_DECLARE(not_instantmessage);
LV_IMG_DECLARE(not_missedcall);
LV_IMG_DECLARE(not_schedule);
LV_IMG_DECLARE(not_sms);
LV_IMG_DECLARE(not_unknown);

LV_IMG_DECLARE(icon_phone);
*/

Notifications::Notifications(DisplayApp *app, Pinetime::Controllers::NotificationManager &notificationManager) :
        Screen(app), notificationManager{notificationManager} {

  

  // Set the background to Black
  lv_obj_set_style_local_bg_color(lv_scr_act(), LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_make(0, 0, 0));

  notificationManager.ClearNewNotificationFlag();
  
  auto notification = notificationManager.GetLastNotification();

  if(notification.valid) {
    currentId = notification.id;
    currentItem = std::make_unique<NotificationItem>(CategoryToString(notification.category), notification, notification.index, notificationManager.NbNotifications());
    validDisplay = true;
  } else {
    //currentItem = std::make_unique<NotificationItem>("Notification", NULL, 0, notificationManager.NbNotifications(), Modes::Preview);

    lv_obj_t * not_img = lv_img_create(lv_scr_act(), NULL);
    lv_img_set_src(not_img, "F:/not_unknown.bin");
    lv_obj_align(not_img, NULL, LV_ALIGN_CENTER, 0, -70);

    lv_obj_t* label = lv_label_create(lv_scr_act(), nullptr);   
    lv_label_set_recolor(label, true); 
    lv_label_set_text_static(label, "#0000FF Notification#\n\nNo notifications\nto display.");
    lv_label_set_align(label, LV_LABEL_ALIGN_CENTER);
    lv_obj_align(label, NULL, LV_ALIGN_CENTER, 0, 20);
    
  }


}

Notifications::~Notifications() {
  currentItem.reset(nullptr);
  lv_obj_clean(lv_scr_act());
}


// Unknown, Email, IncomingCall, MissedCall, Sms, Schedule, InstantMessage
char const *Notifications::CategoriesString[] = {
        "Unknown",
        "e-Mail",
        "Incoming Call",
        "Missed Call",
        "Sms",        
        "Schedule",
        "Instant Message"
};

char const *Notifications::CategoriesIcon[] = {
        "F:/not_unknown.bin",
        "F:/not_email.bin",
        "F:/not_missedcall.bin",
        "F:/not_missedcall.bin",
        "F:/not_unknown.bin",
        "F:/not_schedule.bin",
        "F:/not_unknown.bin"
};

lv_color_t const Notifications::CategoriesColor[] = {
        lv_color_hex(0x8cc44c),
        lv_color_hex(0x0d89e5),
        lv_color_hex(0x4aad4d),
        lv_color_hex(0x4aad4d),
        lv_color_hex(0x1097f3),
        lv_color_hex(0x8bc7f9),
        lv_color_hex(0xfec100)
};

const char* Notifications::CategoryToString( Controllers::NotificationManager::Categories category ) {
  return Notifications::CategoriesString[(uint8_t)category];
}

bool Notifications::Refresh() {

  return running;
}

bool Notifications::OnTouchEvent(Pinetime::Applications::TouchEvents event) {
  switch (event) {
    case Pinetime::Applications::TouchEvents::SwipeDown: {
        Controllers::NotificationManager::Notification previousNotification;
        if(validDisplay)
          previousNotification = notificationManager.GetPrevious(currentId);
        else
          previousNotification = notificationManager.GetLastNotification();

        if (!previousNotification.valid) {        
          
          return true;
        }

        validDisplay = true;
        currentId = previousNotification.id;
        currentItem.reset(nullptr);
        app->SetFullRefresh(DisplayApp::FullRefreshDirections::Down);
        currentItem = std::make_unique<NotificationItem>(CategoryToString(previousNotification.category), previousNotification,  previousNotification.index, notificationManager.NbNotifications());
      }
      return true;
    case Pinetime::Applications::TouchEvents::SwipeUp: {
        Controllers::NotificationManager::Notification nextNotification;
        if(validDisplay)
          nextNotification = notificationManager.GetNext(currentId);
        else
          nextNotification = notificationManager.GetLastNotification();

        if (!nextNotification.valid) {
          running = false;
          return false;
        }

        validDisplay = true;
        currentId = nextNotification.id;
        currentItem.reset(nullptr);
        app->SetFullRefresh(DisplayApp::FullRefreshDirections::Up);
        currentItem = std::make_unique<NotificationItem>(CategoryToString(nextNotification.category), nextNotification,  nextNotification.index, notificationManager.NbNotifications());
      }
      return true;
      
    case Pinetime::Applications::TouchEvents::LongTap : 
      /*
      // delete ???
      notificationManager.Delete(currentId);*/
      return true;

    default:
      return false;
  }
}

Notifications::NotificationItem::NotificationItem(const char *title, Controllers::NotificationManager::Notification &msg, uint8_t notifNr, uint8_t notifNb)
        : msg{msg}, notifNr{notifNr}, notifNb{notifNb} {

  // Set the background to Black
  //lv_obj_set_style_local_bg_color(lv_scr_act(), LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_make(0, 0, 0));

  lv_obj_t* container1 = lv_cont_create(lv_scr_act(), NULL);

  lv_obj_set_style_local_bg_color(container1, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x222222));
  lv_obj_set_style_local_pad_all(container1, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, 10);
  lv_obj_set_style_local_pad_inner(container1, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, 5);
  lv_obj_set_style_local_border_width(container1, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, 0);

  lv_obj_set_pos(container1, 0, 50);
  lv_obj_set_width(container1, 240);
  lv_obj_set_height(container1, 190);
  //lv_obj_set_auto_realign(container1, true);                  
  //lv_obj_align_origo(container1, NULL, LV_ALIGN_CENTER, 0, 0);
  //lv_cont_set_fit(container1, LV_FIT_MAX);
  lv_cont_set_layout(container1, LV_LAYOUT_COLUMN_LEFT);

  if ( msg.valid ) {

    lv_obj_t * not_img = lv_img_create(lv_scr_act(), NULL);
    lv_img_set_src(not_img, Notifications::CategoriesIcon[(uint8_t)msg.category]);
    lv_obj_align(not_img, NULL, LV_ALIGN_IN_TOP_LEFT, 5, 5);

    lv_obj_t* alert_time = lv_label_create(lv_scr_act(), nullptr);

    lv_obj_set_style_local_text_color(alert_time, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x888888));
    lv_label_set_text_fmt(alert_time, "%s:%s", msg.hour.data(), msg.minute.data());
    lv_obj_align(alert_time, NULL, LV_ALIGN_IN_TOP_MID, 0, 16);

    lv_obj_t* alert_count = lv_label_create(lv_scr_act(), nullptr);    
    lv_label_set_text_fmt(alert_count, "%i/%i", notifNr, notifNb);
    lv_obj_align(alert_count, NULL, LV_ALIGN_IN_TOP_RIGHT, 0, 16);

    lv_obj_t* alert_type = lv_label_create(lv_scr_act(), nullptr);
    lv_obj_set_style_local_text_color(alert_type, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x888888));   
    lv_label_set_text(alert_type, title);
    lv_obj_align(alert_type, NULL, LV_ALIGN_IN_BOTTOM_RIGHT, 0, 0); 

    lv_obj_t* alert_subject = lv_label_create(container1, nullptr);
    lv_obj_set_style_local_text_color(alert_subject, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, Notifications::CategoriesColor[(uint8_t)msg.category]);
    lv_label_set_long_mode(alert_subject, LV_LABEL_LONG_BREAK);
    lv_obj_set_width(alert_subject, LV_HOR_RES - 20);    
    lv_label_set_text(alert_subject, msg.subject.data());
    //lv_obj_align(alert_subject, NULL, LV_ALIGN_IN_TOP_LEFT, 10, 50);

    lv_obj_t* alert_body = lv_label_create(container1, nullptr);    
    lv_obj_set_style_local_text_color(alert_body, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xffffff));
    lv_label_set_long_mode(alert_body, LV_LABEL_LONG_BREAK);
    lv_obj_set_width(alert_body, LV_HOR_RES - 20);
    lv_label_set_text(alert_body, msg.message.data());
    //lv_obj_align(alert_body, alert_subject, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 10);    

  } else {
    lv_obj_t* alert_type = lv_label_create(lv_scr_act(), nullptr);
    //lv_obj_set_width(alert_type, LV_HOR_RES - 20);
    lv_label_set_text_fmt(alert_type, "$s\n\n$s", title, "Invalid alert...");
    lv_obj_align(alert_type, NULL, LV_ALIGN_CENTER, 0, -20);
  }

  lv_obj_t* backgroundLabel = lv_label_create(lv_scr_act(), nullptr);
  lv_label_set_long_mode(backgroundLabel, LV_LABEL_LONG_CROP);
  lv_obj_set_size(backgroundLabel, 240, 240);
  lv_obj_set_pos(backgroundLabel, 0, 0);
  lv_label_set_text_static(backgroundLabel, "");

}


Notifications::NotificationItem::~NotificationItem() {
  lv_obj_clean(lv_scr_act());
}
