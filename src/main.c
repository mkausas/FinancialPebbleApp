#include <pebble.h>
#include "billing_statements.h"

#define BALANCE 0
#define BILL_COUNT 1
#define SECONDS_IN_DAY 86400
#define SECONDS_IN_HOUR 3600
#define SECONDS_IN_MIN 60
#define SECONDS_IN_YEAR 31536000
  
Window* window;

TextLayer *day_layer;
TextLayer *current_balance_text;
TextLayer *total_balance_text;

static struct tm *tn;
TextLayer *text_time_layer;

Layer *bars_layer;
Layer *main_layer;
Layer *window_layer;

static double spent_so_far = 50.00;
static int accounts_grab = 0;
static double set_limit = 60; //TODO: set by settings  
static AppTimer *s_timer;
static bool checkColor = true;

int start_month;
int start_day;
int start_year;
int end_month;
int end_day;
int end_year;
int budget;


//change to new window
static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  billing_start();
}

void click_config_provider(void *context) {
  // Register the ClickHandlers
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
}

//Spits out string with hours and days according
char *secToMinHrDay(int seconds){
  APP_LOG(APP_LOG_LEVEL_INFO, "secToMinHrDay");
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Calc seconds: %d", seconds);
  int days = seconds/SECONDS_IN_DAY;
  int temp1 = seconds%SECONDS_IN_DAY;
  
  int hours = temp1/SECONDS_IN_HOUR;
  int temp2 = temp1%SECONDS_IN_HOUR;
  
  int mins = temp2/SECONDS_IN_MIN;
  //seconds = temp2%SECONDS_IN_MIN;
  
  char* toRet = (char *) malloc(30*sizeof(char*));
  snprintf(toRet, 30, "%d days, %d hrs", days, hours);
    APP_LOG(APP_LOG_LEVEL_INFO, "secToMinHrDay toRet = %s", toRet);
  return toRet;
}

//takes a date and returns the seconds in it since 1970
int dateTimeToSec(int year, int month, int day, int hour, int min, int sec){
  int total_seconds = 0;
  if(month==1||month==3||month==5||month==7||month==8||month==10||month==12){
    total_seconds += 31*SECONDS_IN_DAY;
  }else if(month==2){
    total_seconds += 28*SECONDS_IN_DAY;
  }else{
    total_seconds += 30*SECONDS_IN_DAY;
  }
  
  total_seconds += (year-1970)*365*SECONDS_IN_DAY;
  total_seconds += day*SECONDS_IN_DAY;
  total_seconds += hour*SECONDS_IN_HOUR;
  total_seconds += min*SECONDS_IN_MIN;
  total_seconds += sec;
  return total_seconds;
}


void draw_text(){

  int totalBalance = accounts_grab*100;
  int spentBalance = spent_so_far*100;
  
//--------Making time til budget ends work---------------
  //Year, month, day, hour, min, sec of time started
  int start_year = 2015;
  int start_month = 8;
  int start_day = 14;
  int start_hour = 17; //hour in 24hr
  int start_min = 0;
  int start_sec = 0; 
  
  //Year, month, day, hour, min, sec of time started
  int end_year = 2015;
  int end_month = 8;
  int end_day = 17;
  int end_hour = 8; //hour in 24hr
  int end_min = 30;
  int end_sec = 0; 
  
  int start_total_seconds = dateTimeToSec(start_year, start_month, start_day, start_hour, start_min, start_sec);
  int end_total_seconds = dateTimeToSec(end_year, end_month, end_day, end_hour, end_min, end_sec);
  
  char *timeFormatted = secToMinHrDay(end_total_seconds-start_total_seconds);
  text_layer_set_text(text_time_layer, timeFormatted); 
  APP_LOG(APP_LOG_LEVEL_INFO, "timeFormatted: %s", timeFormatted);
  //free(timeFormatted);  //may need to
  //Manual limit
    int budget_left = set_limit*100 - spentBalance;
  
  static char b_dollar[20];
  snprintf(b_dollar,sizeof(b_dollar),"%d",budget_left/100);
  static char b_cent[10];
  snprintf(b_cent, sizeof(b_cent),"%02d",budget_left%100);
  static char budget_balance[20];
  snprintf(budget_balance, sizeof(budget_balance), "%s%s%s%s", "$", b_dollar, ".", b_cent);
  text_layer_set_text(current_balance_text, budget_balance);
  
  //turns how much total left into currency form
  static char t_dollar[20];
  snprintf(t_dollar,sizeof(t_dollar),"%d",totalBalance/100);
  static char t_cent[10];
  snprintf(t_cent, sizeof(t_cent),"%02d",totalBalance%100);
  static char total_balance[20];
  snprintf(total_balance, sizeof(total_balance), "%s%s%s%s", "Balance: $", t_dollar, ".", t_cent);
  text_layer_set_text(total_balance_text, total_balance);
  
  }



void bars_update_callback(Layer *me, GContext* ctx) {
  (void)me;

  graphics_context_set_stroke_color(ctx, GColorBlack);
  
  //Progress Bar
  graphics_draw_line(ctx, GPoint(5, 147),     GPoint(144-7, 147));  //long line, bottom
  graphics_draw_line(ctx, GPoint(5, 147+6),   GPoint(144-7, 147+6)); //short line, right
  graphics_draw_line(ctx, GPoint(4, 148),     GPoint(4, 148+4)); //short line, left
  graphics_draw_line(ctx, GPoint(144-6, 148), GPoint(144-6, 148+4)); //long line, top
  
  int bar_percent; 
  //sets percentage for progress bar
  double percentageLeft = ((double)spent_so_far/set_limit); 
  percentageLeft = ((double)134*percentageLeft); //Percent of bar based on % of balance
  bar_percent = (int)percentageLeft;
  //fills the amount of progress for bar
  graphics_context_set_fill_color(ctx, GColorWhite);
  graphics_fill_rect(ctx, GRect(5, 148, bar_percent, 5), 0, GCornerNone);
  
}



void initialize_text_layers(){
  //Current Account Balance text
  current_balance_text = text_layer_create(GRect(8, 20, 144-16, 20+28)); //GRect(8, 49, 144-16, 49+28));   
  text_layer_set_text_color(current_balance_text, GColorBlack);
  text_layer_set_background_color(current_balance_text, GColorClear);
  text_layer_set_font(current_balance_text, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
  layer_add_child(window_layer, text_layer_get_layer(current_balance_text));
  
  //total account balance text
  total_balance_text = text_layer_create(GRect(8,95,144,95+22));  
  text_layer_set_text_color(total_balance_text,GColorBlack);
  text_layer_set_background_color(total_balance_text,GColorClear);
  text_layer_set_font(total_balance_text, fonts_get_system_font(FONT_KEY_GOTHIC_18));
  layer_add_child(window_layer,text_layer_get_layer(total_balance_text));

  //Clock text
  text_time_layer = text_layer_create(GRect(8, 60, 144, 60+20));  
  text_layer_set_text_color(text_time_layer, GColorBlack);
  text_layer_set_background_color(text_time_layer, GColorClear);
  text_layer_set_font(text_time_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  text_layer_set_text(text_time_layer, "00:00");
  layer_add_child(window_layer, text_layer_get_layer(text_time_layer));
}

void progress_update_callback(Layer *me, GContext* ctx) {
  (void)me;
  time_t now = time(NULL);
  tn = localtime(&now);
  graphics_context_set_stroke_color(ctx, GColorWhite);
  
  draw_text();
  
  //Calculate the percentage of alotted funds based on current spending amount
  
}


//Could recomment
// void handle_minute_tick(struct tm *tick_time, TimeUnits units_changed) {

//   static char time_text[] = "00:00";
//   char *time_format;

//   //Time
//   if (clock_is_24h_style()) {
//     time_format = "%R";
//   } else {
//     time_format = "%I:%M";
//   }

 
//   strftime(time_text, sizeof(time_text), time_format, tick_time);

//   if (!clock_is_24h_style() && (time_text[0] == '0')) {
//     memmove(time_text, &time_text[1], sizeof(time_text) - 1);
//   }

//   //text_layer_set_text(text_time_layer, time_text);

  
//   //Redraw layer
//   layer_mark_dirty(main_layer);
// }

GColor getRightColor(){
  int to_switch_by = (spent_so_far*100)/(set_limit*100);
  switch(to_switch_by%10){
    case 0:
      return GColorGreen;
      break;
    case 1:
      return GColorGreen;
      break;
    case 2:
      return GColorBrightGreen;
      break;
    case 3:
      return GColorSpringBud;
      break;
    case 4:
      return GColorIcterine;
      break;
    case 5:
      return GColorPastelYellow;
      break;
    case 6:
      return GColorYellow;
      break;
    case 7:
      return GColorRajah;
      break;
    case 8:
      return GColorChromeYellow;
      break;
    case 9:
      return GColorOrange;
      break;
    case 10:
      return GColorRed;
      break;
  }
  return GColorWhite;
}


void timer_callback(void *context){
  layer_mark_dirty(main_layer);
  if(checkColor){
    window_set_background_color(window, getRightColor());
    checkColor = !checkColor;
  }
  s_timer = app_timer_register(4000, timer_callback, NULL);
}

static void handle_init(void) {

  window = window_create();
  window_stack_push(window, true /* Animated */);
  window_set_background_color(window, GColorBlack);

  window_layer = window_get_root_layer(window);
  
  initialize_text_layers();

  bars_layer = layer_create(layer_get_frame(window_layer));
  layer_set_update_proc(bars_layer, bars_update_callback);
  layer_add_child(window_layer, bars_layer);

  main_layer = layer_create(layer_get_frame(window_layer));
  layer_set_update_proc(main_layer, progress_update_callback);
  layer_add_child(window_layer, main_layer);
  
  //tick_timer_service_subscribe(MINUTE_UNIT, handle_minute_tick);
  window_set_click_config_provider(window, click_config_provider);
  s_timer = app_timer_register(4000, timer_callback, NULL);
}


static void handle_destroy(void) {
  //Texts

  //Current Balance
  text_layer_destroy(current_balance_text);

  //Clock text
  text_layer_destroy(text_time_layer);
  
  layer_destroy(bars_layer);
  
  layer_destroy(main_layer);
  tick_timer_service_unsubscribe();
  layer_destroy(window_layer);
  app_timer_cancel(s_timer);
  window_destroy(window);
}


 

static Bill *bills;
Bill getBills(int index) {
  return bills[index];
}
int numBills = 0;
int getNumBills(void) {
  return numBills;
}

static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
  printf("MADE IT\n");
  
  // start reading default items
  Tuple *tuple;
  
  tuple = dict_find(iterator, 300);

  int config = (int) tuple->value->int32;

  printf("config = %d", config);
  
  if (config == 0) {

    // balance
    tuple = dict_find(iterator, 0);
    int accountBalance = (int) tuple->value->int32; 
    accounts_grab = accountBalance;
    printf("balance tuple value = %d", accountBalance);
    
    // num bills
    tuple = dict_find(iterator, 1);
    numBills = (int) tuple->value->int32;
    printf("bill count tuple value = %d", numBills);
    bills = (Bill *) calloc(numBills, sizeof(Bill));
    
    for (int i = 0; i < numBills; i++) {
      printf("Bill number: %d\n", i);
      
      int startingPoint = (i * 5) + 2;
      
      // payee
      tuple = dict_find(iterator, startingPoint);
      bills[i].payee[0] = '\0';
      snprintf(bills[i].payee, sizeof(bills[i].payee), "%s", tuple->value->cstring);
      printf("Payee = %s\n", bills[i].payee);
      
      // payment amount
      tuple = dict_find(iterator, startingPoint + 1);
      bills[i].amount = (int) tuple->value->int32;
      bills[i].amt[0] = '\0';
      snprintf(bills[i].amt,sizeof(bills[i].amt),"%s%d","$",bills[i].amount);
      printf("Amount = %d\n", bills[i].amount);
  
      // month
      tuple = dict_find(iterator, startingPoint + 2);
      bills[i].month = (int) tuple->value->int32;
      bills[i].mnth[0] = '\0';
      snprintf(bills[i].mnth,sizeof(bills[i].mnth),"%02d",bills[i].month);
      printf("Month = %d\n", bills[i].month);
      
      // day
      tuple = dict_find(iterator, startingPoint + 3);
      bills[i].day = (int) tuple->value->int32;
      bills[i].dy[0] = '\0';
      snprintf(bills[i].dy,sizeof(bills[i].dy),"%02d",bills[i].day);
      printf("Day = %d\n", bills[i].day);
      
      // year 
      tuple = dict_find(iterator, startingPoint + 4);
      bills[i].yr[0] = '\0';
      bills[i].year = (int) tuple->value->int32;
      snprintf(bills[i].yr,sizeof(bills[i].yr),"%02d",bills[i].year);
      printf("Year = %d\n", bills[i].year);
  
      // title including amount paid, payee
      bills[i].title[0] = '\0';
      snprintf(bills[i].title, sizeof(bills[i].title), "%s%s%s", bills[i].amt, "     ", bills[i].payee);
      // subtitle including clean date
      bills[i].fulldate[0] = '\0';
      snprintf(bills[i].fulldate, sizeof(bills[i].fulldate), "%02d%02d%02d", bills[i].day, bills[i].month, bills[i].year); //"%s%s%s", bills[i].mnth, bills[i].dy, bills[i].yr);
      printf("date = %s\n", bills[i].fulldate);
    }
  } else {
      printf("MADE IT5\n");

    // start_month
    tuple = dict_find(iterator, 1000);
    start_month = (int) tuple->value->int32; 
    printf("start_month tuple value = %d", start_month);
  
    // start_day
    tuple = dict_find(iterator, 1001);
    start_day = (int) tuple->value->int32; 
    printf("start_day tuple value = %d", start_day);
  
    // start_year
    tuple = dict_find(iterator, 1002);
    start_year = (int) tuple->value->int32; 
    printf("start_day tuple value = %d", start_year);
  
    // end_month
    tuple = dict_find(iterator, 1003);
    end_month = (int) tuple->value->int32; 
    printf("end_month tuple value = %d", end_month);
  
    // end_day
    tuple = dict_find(iterator, 1004);
    end_day = (int) tuple->value->int32; 
    printf("end_day tuple value = %d", end_day);
  
    // start_year
    tuple = dict_find(iterator, 1005);
    end_year = (int) tuple->value->int32; 
    printf("end_year tuple value = %d", end_year);
  
    // end_month
    tuple = dict_find(iterator, 1006);
    budget = (int) tuple->value->int32; 
    printf("budget tuple value = %d", budget);
  }
  
  
  layer_mark_dirty(main_layer);
}
  
static void inbox_dropped_callback(AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
}

static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
}

static void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}




int main(void) {
   handle_init();

  // Register callbacks
  app_message_register_inbox_received(inbox_received_callback);
  app_message_register_inbox_dropped(inbox_dropped_callback);
  app_message_register_outbox_failed(outbox_failed_callback);
  app_message_register_outbox_sent(outbox_sent_callback);
  
  // Open AppMessage
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
  

  app_event_loop();
  
   handle_destroy();
}