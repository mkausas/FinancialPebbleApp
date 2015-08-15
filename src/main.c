#include <pebble.h>


Window* window;


TextLayer *day_layer;
TextLayer *current_balance;

static struct tm *tn;
TextLayer *text_time_layer;

Layer *bars_layer;
Layer *main_layer;
Layer *window_layer;

void bars_update_callback(Layer *me, GContext* ctx) {
  (void)me;

  graphics_context_set_stroke_color(ctx, GColorWhite);
  
  //Day
  graphics_draw_line(ctx, GPoint(5, 24),      GPoint(144-7, 24));
  graphics_draw_line(ctx, GPoint(5, 24+6),    GPoint(144-7, 24+6));
  graphics_draw_line(ctx, GPoint(4, 25),      GPoint(4, 25+4));
  graphics_draw_line(ctx, GPoint(144-6, 25),  GPoint(144-6, 25+4));
  
}


void progress_update_callback(Layer *me, GContext* ctx) {
  (void)me;
time_t now = time(NULL);
	tn = localtime(&now);
  graphics_context_set_stroke_color(ctx, GColorWhite);
  
  
  //Progress bars
  //Calculate the percentage of alotted funds based on current spending amount
  //Year
  float year;
  int year_percent;
  
  year = tn->tm_yday;
  year = ((double)100/366)*year; //Percent of year

  
  //caculate the percent of bar based on percentage of funds used
  //year
  year = ((double)134/100)*year; //Percent of bar based on % of year
  year_percent = (int)year;

  graphics_context_set_fill_color(ctx, GColorWhite);
  graphics_fill_rect(ctx, GRect(5, 25, year_percent, 5), 0, GCornerNone);

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
  
  
  
  
  //Texts

    //Progress Bar text
    day_layer = text_layer_create(GRect(8, 7, 144-16, 7+12));   //sets the location of the text on the progress bar
    text_layer_set_text_color(day_layer, GColorWhite);
    text_layer_set_background_color(day_layer, GColorClear);
    text_layer_set_font(day_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14));
    text_layer_set_text(day_layer, "Progress Bar");
    layer_add_child(window_layer, text_layer_get_layer(day_layer));
  
    //Current Account Balance text
    current_balance = text_layer_create(GRect(8, 69, 144-16, 69+12));
    text_layer_set_text_color(current_balance, GColorWhite);
    text_layer_set_background_color(current_balance, GColorClear);
    text_layer_set_font(current_balance, fonts_get_system_font(FONT_KEY_GOTHIC_14));
    text_layer_set_text(current_balance, "Current Balance");
    layer_add_child(window_layer, text_layer_get_layer(current_balance));

  //Clock text
  text_time_layer = text_layer_create(GRect(0, 127, 144, 127+26));
  text_layer_set_text_color(text_time_layer, GColorWhite);
  text_layer_set_background_color(text_time_layer, GColorClear);
  text_layer_set_font(text_time_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
  text_layer_set_text_alignment(text_time_layer, GTextAlignmentCenter);
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

    //Day
    text_layer_destroy(day_layer);
  
    //Current Balance
    text_layer_destroy(current_balance);

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