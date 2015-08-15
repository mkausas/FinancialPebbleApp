#include <pebble.h>


Window* window;


TextLayer *day_layer;
TextLayer *current_balance_text;
TextLayer *total_balance_text;

static struct tm *tn;
TextLayer *text_time_layer;

Layer *bars_layer;
Layer *main_layer;
Layer *window_layer;


void bars_update_callback(Layer *me, GContext* ctx) {
  (void)me;

  graphics_context_set_stroke_color(ctx, GColorWhite);
  
  //Progress Bar
  graphics_draw_line(ctx, GPoint(5, 147),     GPoint(144-7, 147));  //long line, bottom
  graphics_draw_line(ctx, GPoint(5, 147+6),   GPoint(144-7, 147+6)); //short line, right
  graphics_draw_line(ctx, GPoint(4, 148),     GPoint(4, 148+4)); //short line, left
  graphics_draw_line(ctx, GPoint(144-6, 148), GPoint(144-6, 148+4)); //long line, top
  
}



void progress_update_callback(Layer *me, GContext* ctx) {
  (void)me;
time_t now = time(NULL);
	tn = localtime(&now);
  graphics_context_set_stroke_color(ctx, GColorWhite);
  
  

  //Calculate the percentage of alotted funds based on current spending amount
  double capital_one_grab = 150.50;
  int totalBalance = capital_one_grab*100;
  int spentBalance = 50*100;
  int bar_percent; 
  int total_left = totalBalance - spentBalance;
  
  //Manual limit
  double set_limit = 60;
  int budget_left = set_limit*100 - spentBalance;
  
  
  
  //turns budget left into currency form
  static char b_dollar[20];
  snprintf(b_dollar,sizeof(b_dollar),"%d",budget_left/100);
  static char b_cent[10];
  snprintf(b_cent, sizeof(b_cent),"%02d",budget_left%100);
  static char final_balance[20];
  snprintf(final_balance, sizeof(final_balance), "%s%s%s%s", "$", b_dollar, ".", b_cent);
  text_layer_set_text(current_balance_text, final_balance);
  
  
  //turns how much total left into currency form
  static char t_dollar[20];
  snprintf(t_dollar,sizeof(t_dollar),"%d",total_left/100);
  static char t_cent[10];
  snprintf(t_cent, sizeof(t_cent),"%02d",total_left%100);
  static char total_balance[20];
  snprintf(total_balance, sizeof(total_balance), "%s%s%s%s", "Total: $", t_dollar, ".", t_cent);
  text_layer_set_text(total_balance_text, total_balance);
  
  
  
  //sets percentage for progress bar
  double percentageLeft = ((double)100/totalBalance)*total_left; 
  percentageLeft = ((double)134/100)*percentageLeft; //Percent of bar based on % of balance
  bar_percent = (int)percentageLeft;
  //fills the amount of progress for bar
  graphics_context_set_fill_color(ctx, GColorWhite);
  graphics_fill_rect(ctx, GRect(5, 148, bar_percent, 5), 0, GCornerNone);

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

  text_layer_set_text(text_time_layer, time_text);

  
    //Redraw layer
  layer_mark_dirty(main_layer);
}


static void handle_init(void) {

  window = window_create();
  window_stack_push(window, true /* Animated */);
  window_set_background_color(window, GColorBlack);

	  window_layer = window_get_root_layer(window);
  
    //Current Account Balance text
  current_balance_text = text_layer_create(GRect(8, 49, 144-16, 49+28));   
  text_layer_set_text_color(current_balance_text, GColorWhite);
  text_layer_set_background_color(current_balance_text, GColorClear);
  text_layer_set_font(current_balance_text, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
  layer_add_child(window_layer, text_layer_get_layer(current_balance_text));
  
  //total account balance text
  total_balance_text = text_layer_create(GRect(8,120,144,120+22));
  text_layer_set_text_color(total_balance_text,GColorWhite);
  text_layer_set_background_color(total_balance_text,GColorClear);
  text_layer_set_font(total_balance_text, fonts_get_system_font(FONT_KEY_GOTHIC_18));
  layer_add_child(window_layer,text_layer_get_layer(total_balance_text));


  //Clock text
  text_time_layer = text_layer_create(GRect(8, 87, 144, 87+20));
  text_layer_set_text_color(text_time_layer, GColorWhite);
  text_layer_set_background_color(text_time_layer, GColorClear);
  text_layer_set_font(text_time_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  
  text_layer_set_text(text_time_layer, "00:00");
  layer_add_child(window_layer, text_layer_get_layer(text_time_layer));


  bars_layer = layer_create(layer_get_frame(window_layer));
  layer_set_update_proc(bars_layer, bars_update_callback);
  layer_add_child(window_layer, bars_layer);

  main_layer = layer_create(layer_get_frame(window_layer));
  layer_set_update_proc(main_layer, progress_update_callback);
  layer_add_child(window_layer, main_layer);
	
  tick_timer_service_subscribe(MINUTE_UNIT, handle_minute_tick);
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


int main(void) {
   handle_init();

   app_event_loop();
	
   handle_destroy();
}