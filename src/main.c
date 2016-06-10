#include <pebble.h>
#include <pebble_fonts.h>


#define KEY_COLOR_RED     0
#define KEY_COLOR_GREEN   1
#define KEY_COLOR_BLUE    2
#define KEY_TEMP_C        3
#define KEY_TEMP_F        4
#define KEY_LOCATION      5
#define KEY_DESCRIPTION   6

static Window *s_main_window;
static TextLayer *s_hour_layer;
static TextLayer *s_minutes_layer;
static TextLayer *s_temp_layer;
static TextLayer *s_short_layer;
static GFont s_hour_font;
static GFont s_minutes_font;
static BitmapLayer *logo_layer;
static GBitmap *logo_bitmap;

static void inbox_received_handler(DictionaryIterator *iter, void *context) {
  
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Inbox called!");
  // Weather data?
  Tuple *temp_c_t = dict_find(iter, KEY_TEMP_C);
  Tuple *temp_f_t = dict_find(iter, KEY_TEMP_F);
  Tuple *location_t = dict_find(iter, KEY_LOCATION);
  Tuple *description_t = dict_find(iter, KEY_DESCRIPTION);
  
  // Color Scheme?
  Tuple *color_red_t = dict_find(iter, KEY_COLOR_RED);
  Tuple *color_green_t = dict_find(iter, KEY_COLOR_GREEN);
  Tuple *color_blue_t = dict_find(iter, KEY_COLOR_BLUE);
  
  if(color_red_t && color_green_t && color_blue_t) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Got new Background RGB values!");
  // Apply the color, if available
  #if defined(PBL_BW)
    window_set_background_color(s_main_window, GColorWhite);
  #elif defined(PBL_COLOR)
    int red = color_red_t->value->int32;
    int green = color_green_t->value->int32;
    int blue = color_blue_t->value->int32;
  
    // Persist values
    persist_write_int(KEY_COLOR_RED, red);
    persist_write_int(KEY_COLOR_GREEN, green);
    persist_write_int(KEY_COLOR_BLUE, blue);
  
    GColor bg_color = GColorFromRGB(red, green, blue);
    window_set_background_color(s_main_window, bg_color);
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Set new background color!");
  #endif
  }
  
// int temp_c = temp_c_t->value->int32;
// int temp_f = temp_f_t->value->int32;

// char s_temp_long[] = (temp_c + "\u00B0" + "C\n" + location);
// char s_short_long[] = description;
  
  text_layer_set_text(s_temp_layer, &temp_c_t);
  text_layer_set_text(s_short_layer, &description_t);
  
}

static void update_time() {
	time_t temp = time(NULL);
	struct tm *tick_time = localtime(&temp);
	
	static char buffer[] = "00";
	static char buffer2[] = "00";
	
	if (clock_is_24h_style() == true) {
		//24-hour time
		strftime(buffer, sizeof("00"), "%H", tick_time);
		strftime(buffer2, sizeof("00"), "%M", tick_time);
	} else {
		//12-hour time
		strftime(buffer, sizeof("00"), "%I", tick_time);
		strftime(buffer2, sizeof("00"), "%M", tick_time);
	}
	
	//Display hours on hour layer
	text_layer_set_text(s_hour_layer, buffer);
	//Display minutes on minutes layer
	text_layer_set_text(s_minutes_layer, buffer2);
}

static void main_window_load(Window *window) {
	
#if defined(PBL_BW)
    window_set_background_color(s_main_window, GColorWhite);
#elif defined(PBLE_COLOR)
  int red = persist_read_int(KEY_COLOR_RED);
  int green = persist_read_int(KEY_COLOR_GREEN);
  int blue = persist_read_int(KEY_COLOR_BLUE);
  
  GColor bg_color = GColorFromRGB(red, green, blue);
  window_set_background_color(s_main_window, bg_color);
#endif
  
  // Get the Window's root layer and the bounds
  GRect bounds = layer_get_bounds(window_get_root_layer(window));
  
  // Load the image
  #ifdef PBL_COLOR
    logo_bitmap = gbitmap_create_with_resource(RESOURCE_ID_LOGO);
  #else
    logo_bitmap = gbitmap_create_with_resource(RESOURCE_ID_LOGO_BW);
  #endif

// Create a BitmapLayer
logo_layer = bitmap_layer_create(bounds);

// Set the bitmap and center it
bitmap_layer_set_bitmap(logo_layer, logo_bitmap);
bitmap_layer_set_alignment(logo_layer, GAlignCenter);
bitmap_layer_set_compositing_mode(logo_layer, GCompOpSet);

// Add to the Window
layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(logo_layer));
	
	// Set font hour TextLayer
	s_hour_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_SYNC_BOLD_50));
	// Set font minutes TextLayer
	s_minutes_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_SYNC_BOLD_50));
	
	//Hour Layer
  #if defined(PBL_RECT) 
	  s_hour_layer = text_layer_create(GRect (0, 30, 144, 54));
  #elif defined(PBL_ROUND)
    s_hour_layer = text_layer_create(GRect (0, 35, 180, 54));
  #endif
	text_layer_set_background_color(s_hour_layer, GColorClear);
  text_layer_set_text_color(s_hour_layer, GColorBlack);
	text_layer_set_text(s_hour_layer, "00");
	
	//Minute Layer
	#if defined(PBL_RECT) 
    s_minutes_layer = text_layer_create(GRect (0, 74, 144, 54));
  #elif defined(PBL_ROUND) 
    s_minutes_layer = text_layer_create(GRect (0, 79, 180, 54));
  #endif
	text_layer_set_background_color(s_minutes_layer, GColorClear);
	text_layer_set_text_color(s_minutes_layer, GColorBlack);
	text_layer_set_text(s_minutes_layer, "00");
	
  //Temp layer
  #if defined(PBL_RECT)
    s_temp_layer = text_layer_create(GRect (0, 0, 144, 45));
  #elif defined(PBL_ROUND)
    s_temp_layer = text_layer_create(GRect (0, 0, 180, 45));
  #endif
  text_layer_set_background_color(s_temp_layer, GColorClear);
  text_layer_set_text_color(s_temp_layer, GColorBlack);
  text_layer_set_text(s_temp_layer, "Ready!");
  
  //Short Layer
  #if defined(PBL_RECT)
    s_short_layer = text_layer_create(GRect (0, 130, 144, 18));
  #elif defined(PBL_ROUND)
    s_short_layer = text_layer_create(GRect (0, 145, 180, 30));
  #endif
  text_layer_set_background_color(s_short_layer, GColorClear);
  text_layer_set_text_color(s_short_layer, GColorBlack);
  text_layer_set_text(s_short_layer, "\U0001F61C");
  
	//Make it look more like a watch
	text_layer_set_font(s_hour_layer, s_hour_font);
	text_layer_set_font(s_minutes_layer, s_minutes_font);
  text_layer_set_font(s_temp_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18));
  text_layer_set_font(s_short_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18));
	text_layer_set_text_alignment(s_hour_layer, GTextAlignmentCenter);
	text_layer_set_text_alignment(s_minutes_layer, GTextAlignmentCenter);
  text_layer_set_text_alignment(s_temp_layer, GTextAlignmentCenter);
  text_layer_set_text_alignment(s_short_layer, GTextAlignmentCenter);
	
	//Add hour, miuntes, and weather layers as child windows to the Window's root layer
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_hour_layer));
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_minutes_layer));
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_temp_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_short_layer));
}

static void main_window_unload(Window *window) {
	
	//Unload GFonts
	fonts_unload_custom_font(s_hour_font);
	fonts_unload_custom_font(s_minutes_font);
	
	//Destroy hour layer
	text_layer_destroy(s_hour_layer);
	//Destroy minutes layer
	text_layer_destroy(s_minutes_layer);
	
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
 update_time();
}

static void init() {
	tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
	s_main_window = window_create();
	window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });
	window_stack_push(s_main_window, true);
	update_time();
  
  app_message_register_inbox_received(inbox_received_handler);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Calling Inbox!");
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());

}

static void deinit() {
	//Destroy Window
	window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}