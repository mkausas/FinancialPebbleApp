#include <pebble.h>
#include "billing_statements.h"
#include "main.h"
  

#define NUM_MENU_ICONS 3
//#define NUM_SECOND_MENU_ITEMS 1

static Window *s_main_window;
static MenuLayer *s_menu_layer;
// static GBitmap *s_menu_icons[NUM_MENU_ICONS];
// static GBitmap *s_background_bitmap;

static int s_current_icon = 0;

static uint16_t menu_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "getnum:%d", getNumBills());
    return getNumBills();
}

static void menu_draw_header_callback(GContext* ctx, const Layer *cell_layer, uint16_t section_index, void *data) {
  // Draw title text in the section header
  menu_cell_basic_header_draw(ctx, cell_layer, "Billing Statements");
}
static void menu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "enter draw row");

    int row = cell_index->row;
    APP_LOG(APP_LOG_LEVEL_DEBUG, "crash");

//     APP_LOG(APP_LOG_LEVEL_DEBUG, "gettitle:%s, getdate:%s", getBills(row).title, getBills(row).fulldate);

    menu_cell_basic_draw(ctx, cell_layer, getBills(row).title, NULL, NULL);
}

// static void menu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data) {
//   // Determine which section we're going to draw in
//   switch (cell_index->section) {
//     case 0:
//       // Use the row to specify which item we'll draw
//       switch (cell_index->row) {
//         case 0:
//           // This is a basic menu item with a title and subtitle
// //           printf("%s\n", getBills(cell_index->row).fulldate);
//           menu_cell_basic_draw(ctx, cell_layer, getBills(cell_index->row).title, NULL,
//                                //getBills(cell_index->row).fulldate, 
//                                NULL);
//           break;
//         case 1:
//           // This is a basic menu icon with a cycling icon
//           menu_cell_basic_draw(ctx, cell_layer, "Icon Item", "Select to cycle", NULL);
//           break;
//       }
//       break;
//   }
// }

static void menu_select_click(MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context){
  APP_LOG(APP_LOG_LEVEL_DEBUG, "clicked!");
}

static void main_window_load(Window *window) {
  printf("main_window_load\n");
  
  //prepare to initialize the menu layer
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_frame(window_layer);

      APP_LOG(APP_LOG_LEVEL_DEBUG, "before create menu layer");

  // Create the menu layer
  s_menu_layer = menu_layer_create(bounds);
  menu_layer_set_callbacks(s_menu_layer, NULL, (MenuLayerCallbacks){
    .get_num_rows = menu_get_num_rows_callback,
    .draw_header = menu_draw_header_callback,
    .draw_row = menu_draw_row_callback,
    .select_click = menu_select_click,
  });

  APP_LOG(APP_LOG_LEVEL_DEBUG, "after enter draw row");

  // Bind the menu layer's click config provider to the window for interactivity
  menu_layer_set_click_config_onto_window(s_menu_layer, window);

  APP_LOG(APP_LOG_LEVEL_DEBUG, "after menu layer set click config onto window");
     menu_layer_set_highlight_colors(s_menu_layer, GColorPictonBlue, GColorWhite);
  menu_layer_set_normal_colors(s_menu_layer, GColorWhite, GColorBlack);
  layer_add_child(window_layer, menu_layer_get_layer(s_menu_layer));
  
  APP_LOG(APP_LOG_LEVEL_DEBUG, "after menu layer get layer");

}

static void main_window_unload(Window *window) {
    printf("main_window_unload\n");

  
  // Destroy the menu layer
  menu_layer_destroy(s_menu_layer);

//   // Cleanup the menu icons
//   for (int i = 0; i < NUM_MENU_ICONS; i++) {
//     gbitmap_destroy(s_menu_icons[i]);
//   }

//   gbitmap_destroy(s_background_bitmap);
}

void billing_start() {
  printf("billing_start\n");

  s_main_window = window_create();
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload,
  });
  //window_set_click_config_provider(s_main_window, click_config_provider);
  window_stack_push(s_main_window, true);
}

void billing_end() {  
  printf("billing_end\n");

  window_destroy(s_main_window);
}
