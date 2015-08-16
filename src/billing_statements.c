#include <pebble.h>
#include "billing_statements.h"
#include "main.h"
  

#define NUM_MENU_ICONS 3
//#define NUM_SECOND_MENU_ITEMS 1

static Window *s_main_window;
static MenuLayer *s_menu_layer;
static GBitmap *s_menu_icons[NUM_MENU_ICONS];
static GBitmap *s_background_bitmap;

static int s_current_icon = 0;

static uint16_t menu_get_num_sections_callback(MenuLayer *menu_layer, void *data) {
  return getNumBills();
}

static uint16_t menu_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
    switch (section_index) {
    case 0:
      return getNumBills();
    default:
      return 0;
  }
}

static int16_t menu_get_header_height_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
  return MENU_CELL_BASIC_HEADER_HEIGHT;
}

static void menu_draw_header_callback(GContext* ctx, const Layer *cell_layer, uint16_t section_index, void *data) {
  // Determine which section we're working with
  switch (section_index) {
    case 0:
      // Draw title text in the section header
      menu_cell_basic_header_draw(ctx, cell_layer, "Billing Statements");
      break;
  }
}

static void menu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data) {
  for(int i = 0; i < getNumBills(); i++) {
    if(cell_index->row == i)
      menu_cell_basic_draw(ctx, cell_layer, getBills(i).title, getBills(i).fulldate, NULL);
  }    
}


static void main_window_load(Window *window) {

  //prepare to initialize the menu layer
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_frame(window_layer);

  // Create the menu layer
  s_menu_layer = menu_layer_create(bounds);
  menu_layer_set_callbacks(s_menu_layer, NULL, (MenuLayerCallbacks){
    .get_num_sections = menu_get_num_sections_callback,
    .get_num_rows = menu_get_num_rows_callback,
    .get_header_height = menu_get_header_height_callback,
    .draw_header = menu_draw_header_callback,
    .draw_row = menu_draw_row_callback,
  });

  // Bind the menu layer's click config provider to the window for interactivity
  menu_layer_set_click_config_onto_window(s_menu_layer, window);

  layer_add_child(window_layer, menu_layer_get_layer(s_menu_layer));
}

static void main_window_unload(Window *window) {
  // Destroy the menu layer
  menu_layer_destroy(s_menu_layer);

  // Cleanup the menu icons
  for (int i = 0; i < NUM_MENU_ICONS; i++) {
    gbitmap_destroy(s_menu_icons[i]);
  }

  gbitmap_destroy(s_background_bitmap);
}

void billing_start() {
  s_main_window = window_create();
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload,
  });
  //window_set_click_config_provider(s_main_window, click_config_provider);
  window_stack_push(s_main_window, true);
}

void billing_end() {
  window_destroy(s_main_window);
}
