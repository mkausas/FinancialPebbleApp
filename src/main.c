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

static double capital_one_grab = 50.00;
static double set_limit = 60; //TODO: set by settings  

//change to new window
static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  billing_start();
}

void click_config_provider(void *context) {
  // Register the ClickHandlers
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
}

static int accounts_grab =0; //the amount in the account currently

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
  int spentBalance = capital_one_grab*100;
  
  char *timeFormatted = secToMinHrDay(timeDiff);
  text_layer_set_text(text_time_layer, timeFormatted); 
  APP_LOG(APP_LOG_LEVEL_INFO, "timeFormatted: %s", timeFormatted);
  //free(timeFormatted);
  
  //Manual limit
    int budget_left = set_limit*100 - spentBalance;
  
  static char b_dollar[20];
  snprintf(b_dollar,sizeof(b_dollar),"%d",budget_left/100);
  static char b_cent[10];
  snprintf(b_cent, sizeof(b_cent),"%02d",budget_left%100);
  static char budget_balance[20];
  snprintf(budget_balance, sizeof(budget_balance), "%s%s%s%s", "Total: $", b_dollar, ".", b_cent);
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

  graphics_context_set_stroke_color(ctx, GColorWhite);
  
  //Progress Bar
  graphics_draw_line(ctx, GPoint(5, 147),     GPoint(144-7, 147));  //long line, bottom
  graphics_draw_line(ctx, GPoint(5, 147+6),   GPoint(144-7, 147+6)); //short line, right
  graphics_draw_line(ctx, GPoint(4, 148),     GPoint(4, 148+4)); //short line, left
  graphics_draw_line(ctx, GPoint(144-6, 148), GPoint(144-6, 148+4)); //long line, top
  
  int bar_percent; 
  //sets percentage for progress bar
  double percentageLeft = ((double)capital_one_grab/set_limit); 
  percentageLeft = ((double)134*percentageLeft); //Percent of bar based on % of balance
  bar_percent = (int)percentageLeft;
  //fills the amount of progress for bar
  graphics_context_set_fill_color(ctx, GColorWhite);
  graphics_fill_rect(ctx, GRect(5, 148, bar_percent, 5), 0, GCornerNone);
  
}



void initialize_text_layers(){
  //Current Account Balance text
  current_balance_text = text_layer_create(GRect(8, 20, 144-16, 20+28)); //GRect(8, 49, 144-16, 49+28));   
  text_layer_set_text_color(current_balance_text, GColorWhite);
  text_layer_set_background_color(current_balance_text, GColorClear);
  text_layer_set_font(current_balance_text, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
  layer_add_child(window_layer, text_layer_get_layer(current_balance_text));
  
  //total account balance text
  total_balance_text = text_layer_create(GRect(8,95,144,95+22));  
  text_layer_set_text_color(total_balance_text,GColorWhite);
  text_layer_set_background_color(total_balance_text,GColorClear);
  text_layer_set_font(total_balance_text, fonts_get_system_font(FONT_KEY_GOTHIC_18));
  layer_add_child(window_layer,text_layer_get_layer(total_balance_text));

  //Clock text
  text_time_layer = text_layer_create(GRect(8, 60, 144, 60+20));  
  text_layer_set_text_color(text_time_layer, GColorWhite);
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



void handle_minute_tick(struct tm *tick_time, TimeUnits units_changed) {

  static char time_text[] = "00:00";
  char *time_format;

  //Time
  if (clock_is_24h_style()) {
    time_format = "%R";
  } else {
    time_format = "%I:%M";
  }

 
  strftime(time_text, sizeof(time_text), time_format, tick_time);

  if (!clock_is_24h_style() && (time_text[0] == '0')) {
    memmove(time_text, &time_text[1], sizeof(time_text) - 1);
  }

  //text_layer_set_text(text_time_layer, time_text);

  
  //Redraw layer
  layer_mark_dirty(main_layer);
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
  
  tick_timer_service_subscribe(MINUTE_UNIT, handle_minute_tick);
  
  window_set_click_config_provider(window, click_config_provider);
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

  // start reading default items
  Tuple *tuple;
  
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
    snprintf(bills[i].payee, sizeof(bills[i].payee), "%s", tuple->value->cstring);
    printf("Payee = %s\n", bills[i].payee);
    
    // payment amount
    tuple = dict_find(iterator, startingPoint + 1);
    bills[i].amount = (int) tuple->value->int32;
    snprintf(bills[i].amt,sizeof(bills[i].amt),"%s%d","$",bills[i].amount);
    printf("Amount = %d\n", bills[i].amount);

    // month
    tuple = dict_find(iterator, startingPoint + 2);
    bills[i].month = (int) tuple->value->int32;
    snprintf(bills[i].mnth,sizeof(bills[i].mnth),"%02d",bills[i].month);
    printf("Month = %d\n", bills[i].month);
    
    // day
    tuple = dict_find(iterator, startingPoint + 3);
    bills[i].day = (int) tuple->value->int32;
    snprintf(bills[i].dy,sizeof(bills[i].dy),"%02d",bills[i].day);
    printf("Day = %d\n", bills[i].day);
    
    // year 
    tuple = dict_find(iterator, startingPoint + 4);
    bills[i].year = (int) tuple->value->int32;
    snprintf(bills[i].yr,sizeof(bills[i].yr),"%02d",bills[i].year);
    printf("Year = %d\n", bills[i].year);

    // title including amount paid, payee
    snprintf(bills[i].title, sizeof(bills[i].title), "%s%s%s", bills[i].amt, "     ", bills[i].payee);
    // subtitle including clean date
    snprintf(bills[i].fulldate, sizeof(bills[i].fulldate), "%s%s%s%s%s", bills[i].mnth, "/", bills[i].dy, "/", bills[i].yr);
  }
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