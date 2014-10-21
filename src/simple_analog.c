/*
Simple Analog Plus
Based off of Simple Analog by Pebble
*/

#include "simple_analog.h"
#include "pebble.h"
#include "string.h"
#include "stdlib.h"

void bg_update_proc(Layer *layer, GContext *ctx) {

  	graphics_context_set_fill_color(ctx, GColorBlack);
  	graphics_fill_rect(ctx, layer_get_bounds(layer), 0, GCornerNone);

  	graphics_context_set_fill_color(ctx, GColorWhite);
  	for (int i = 0; i < NUM_CLOCK_TICKS; ++i) {
    	gpath_draw_filled(ctx, tick_paths[i]);
    }
}

void hands_update_proc(Layer *layer, GContext *ctx) {
  	GRect bounds = layer_get_bounds(layer);
  	const GPoint center = grect_center_point(&bounds);
	
  	time_t now = time(NULL);
  	struct tm *t = localtime(&now);

  	graphics_context_set_fill_color(ctx, GColorWhite);
  	graphics_context_set_stroke_color(ctx, GColorBlack);

  	gpath_rotate_to(minute_arrow, TRIG_MAX_ANGLE * t->tm_min / 60);
  	gpath_draw_filled(ctx, minute_arrow);
  	gpath_draw_outline(ctx, minute_arrow);

  	gpath_rotate_to(hour_arrow, (TRIG_MAX_ANGLE * (((t->tm_hour % 12) * 6) + (t->tm_min / 10))) / (12 * 6));
  	gpath_draw_filled(ctx, hour_arrow);
  	gpath_draw_outline(ctx, hour_arrow);

  	graphics_context_set_fill_color(ctx, GColorBlack);
  	//graphics_fill_rect(ctx, GRect(bounds.size.w / 2 - 1, bounds.size.h / 2 - 1, 3, 3), 0, GCornerNone);
}

void date_update_proc(Layer *layer, GContext *ctx) {
  	time_t now = time(NULL);
  	struct tm *t = localtime(&now);

  	strftime(day_buffer, sizeof(day_buffer), "%a", t);
  	text_layer_set_text(day_label, day_buffer);

  	strftime(num_buffer, sizeof(num_buffer), "%d", t);
  	text_layer_set_text(num_label, num_buffer);
}

void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
	layer_mark_dirty(window_get_root_layer(window));
}

void bt_handler(bool connected){
	if(boot){
		if(!connected){
			layer_set_hidden(bitmap_layer_get_layer(bt_layer), false);
			vibes_short_pulse();
		}
		else{
			layer_set_hidden(bitmap_layer_get_layer(bt_layer), true);
			vibes_double_pulse();
		}
	}
	else{
		boot = true;
		if(!connected){
			layer_set_hidden(bitmap_layer_get_layer(bt_layer), false);
		}
		else{
			layer_set_hidden(bitmap_layer_get_layer(bt_layer), true);
		}
	}
}

void battery_handler(BatteryChargeState batt){
	if(batt.is_charging){
		layer_set_hidden(bitmap_layer_get_layer(batt_layer), false);
		bitmap_layer_set_bitmap(batt_layer, batt_charge);
		if(!hasVibed){
			if(boot){
				vibes_double_pulse();
			}
			hasVibed = true;
		}
		return;
	}
	hasVibed = false;
	if(batt.charge_percent < 30){
		layer_set_hidden(bitmap_layer_get_layer(batt_layer), false);
		bitmap_layer_set_bitmap(batt_layer, batt_low);
	}
	else{
		layer_set_hidden(bitmap_layer_get_layer(batt_layer), true);
	}
}

void window_load(Window *window) {
  	Layer *window_layer = window_get_root_layer(window);
  	GRect bounds = layer_get_bounds(window_layer);
	
  	simple_bg_layer = layer_create(bounds);
  	layer_set_update_proc(simple_bg_layer, bg_update_proc);
  	layer_add_child(window_layer, simple_bg_layer);

  	date_layer = layer_create(bounds);
  	layer_set_update_proc(date_layer, date_update_proc);
  	layer_add_child(window_layer, date_layer);

  	day_label = text_layer_create(GRect(50, 114, 27, 20));
  	text_layer_set_text(day_label, day_buffer);
  	text_layer_set_background_color(day_label, GColorBlack);
  	text_layer_set_text_color(day_label, GColorWhite);
  	GFont norm18 = fonts_get_system_font(FONT_KEY_GOTHIC_18);
  	text_layer_set_font(day_label, norm18);

  	layer_add_child(date_layer, text_layer_get_layer(day_label));

  	num_label = text_layer_create(GRect(77, 114, 18, 20));

  	text_layer_set_text(num_label, num_buffer);
  	text_layer_set_background_color(num_label, GColorBlack);
  	text_layer_set_text_color(num_label, GColorWhite);
  	GFont bold18 = fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD);
  	text_layer_set_font(num_label, bold18);
	
	update_at_a_glance = text_layer_create(GRect(0, 300, 140, 168));
	text_layer_set_font(update_at_a_glance, fonts_get_system_font(FONT_KEY_GOTHIC_18));
	text_layer_set_background_color(update_at_a_glance, GColorBlack);
	text_layer_set_text_color(update_at_a_glance, GColorWhite);
  	layer_add_child(window_layer, text_layer_get_layer(update_at_a_glance));

  	layer_add_child(date_layer, text_layer_get_layer(num_label));
	
	pebble_logo = bitmap_layer_create(GRect(0, -52, 144, 168));
	bitmap_layer_set_bitmap(pebble_logo, gbitmap_create_with_resource(RESOURCE_ID_PEBBLE_LOGO));
	layer_add_child(window_layer, bitmap_layer_get_layer(pebble_logo));
	
	batt_low = gbitmap_create_with_resource(RESOURCE_ID_BATT_ICON);
	batt_charge = gbitmap_create_with_resource(RESOURCE_ID_BATT_CHARGE);
	
	batt_layer = bitmap_layer_create(GRect(0, 25, 144, 168));
	bitmap_layer_set_bitmap(batt_layer, batt_low);
	layer_add_child(window_layer, bitmap_layer_get_layer(batt_layer));
	
	bt_layer = bitmap_layer_create(GRect(0, -40, 144, 168));
	bitmap_layer_set_bitmap(bt_layer, gbitmap_create_with_resource(RESOURCE_ID_BT));
	layer_add_child(window_layer, bitmap_layer_get_layer(bt_layer));

  	hands_layer = layer_create(bounds);
  	layer_set_update_proc(hands_layer, hands_update_proc);
  	layer_add_child(window_layer, hands_layer);
	
	BatteryChargeState b = battery_state_service_peek();
	battery_handler(b);
	
	bool connected = bluetooth_connection_service_peek();
	bt_handler(connected);
}

void window_unload(Window *window) {
  	layer_destroy(simple_bg_layer);
  	layer_destroy(date_layer);
  	text_layer_destroy(day_label);
  	text_layer_destroy(num_label);
	text_layer_destroy(update_at_a_glance);
  	layer_destroy(hands_layer);
}

void init() {
  	window = window_create();
  	window_set_window_handlers(window, (WindowHandlers) {
    	.load = window_load,
    	.unload = window_unload,
    });

  	day_buffer[0] = '\0';
  	num_buffer[0] = '\0';

  	minute_arrow = gpath_create(&MINUTE_HAND_POINTS);
  	hour_arrow = gpath_create(&HOUR_HAND_POINTS);

  	Layer *window_layer = window_get_root_layer(window);
  	GRect bounds = layer_get_bounds(window_layer);
  	const GPoint center = grect_center_point(&bounds);
  	gpath_move_to(minute_arrow, center);
  	gpath_move_to(hour_arrow, center);

	for (int i = 0; i < NUM_CLOCK_TICKS; ++i){
  	  tick_paths[i] = gpath_create(&ANALOG_BG_POINTS[i]);
    }

	tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
	bluetooth_connection_service_subscribe(&bt_handler);
	battery_state_service_subscribe(battery_handler);
	
  	window_stack_push(window, true);
}

void deinit() {
  	gpath_destroy(minute_arrow);
  	gpath_destroy(hour_arrow);

  	for (int i = 0; i < NUM_CLOCK_TICKS; ++i) {
    	gpath_destroy(tick_paths[i]);
    }

  	tick_timer_service_unsubscribe();
  	window_destroy(window);
}

int main() {
  	init();
  	app_event_loop();
  	deinit();
}