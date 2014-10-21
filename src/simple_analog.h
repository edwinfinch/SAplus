#pragma once
#include <pebble.h>
#define NUM_CLOCK_TICKS 11
	
Layer *simple_bg_layer;

Layer *date_layer;
TextLayer *day_label, *num_label, *update_at_a_glance;
char day_buffer[6], num_buffer[4];

BitmapLayer *pebble_logo, *batt_layer, *bt_layer;
GBitmap *batt_low, *batt_charge;

GPath *minute_arrow, *hour_arrow, *tick_paths[NUM_CLOCK_TICKS];
Layer *hands_layer;
Window *window;

bool boot = false, hasVibed = false;

const struct GPathInfo ANALOG_BG_POINTS[] = {
  { 4,
    (GPoint []) {
      {68, 0},
      {71, 0},
      {71, 12},
      {68, 12}
    }
  },
  { 4, (GPoint []){
      {72, 0},
      {75, 0},
      {75, 12},
      {72, 12}
    }
  },
  { 4, (GPoint []){
      {112, 10},
      {114, 12},
      {108, 23},
      {106, 21}
    }
  },
  { 4, (GPoint []){
      {132, 47},
      {144, 40},
      {144, 44},
      {135, 49}
    }
  },
  { 4, (GPoint []){
      {135, 118},
      {144, 123},
      {144, 126},
      {132, 120}
    }
  },
  { 4, (GPoint []){
      {108, 144},
      {114, 154},
      {112, 157},
      {106, 147}
    }
  },
  { 4, (GPoint []){
      {70, 155},
      {73, 155},
      {73, 167},
      {70, 167}
    }
  },
  { 4, (GPoint []){
      {32, 10},
      {30, 12},
      {36, 23},
      {38, 21}
    }
  },
  { 4, (GPoint []){
      {12, 47},
      {-1, 40},
      {-1, 44},
      {9, 49}
    }
  },
  { 4, (GPoint []){
      {9, 118},
      {-1, 123},
      {-1, 126},
      {12, 120}
    }
  },
  { 4, (GPoint []){
      {36, 144},
      {30, 154},
      {32, 157},
      {38, 147}
    }
  },

};

const GPathInfo MINUTE_HAND_POINTS =
{
  3,
  (GPoint []) {
    { -8, 20 },
    { 8, 20 },
    { 0, -80 }
  }
};

const GPathInfo HOUR_HAND_POINTS = {
  3, (GPoint []){
    {-6, 20},
    {6, 20},
    {0, -60}
  }
};

void stopped(Animation *anim, bool finished, void *context){
    property_animation_destroy((PropertyAnimation*) anim);
}
 
void animate_layer(Layer *layer, GRect *start, GRect *finish, int duration, int delay){
    PropertyAnimation *anim = property_animation_create_layer_frame(layer, start, finish);
     
    animation_set_duration((Animation*) anim, duration);
    animation_set_delay((Animation*) anim, delay);
     
    AnimationHandlers handlers = {
        .stopped = (AnimationStoppedHandler) stopped
    };
    animation_set_handlers((Animation*) anim, handlers, NULL);
     
    animation_schedule((Animation*) anim);
}

void glance(const char *updateText, bool vibrate, int vibrateNum, int animationLength, bool isHalf){
	text_layer_set_text(update_at_a_glance, updateText);
	if(vibrate){
		switch(vibrateNum){
			case 1:
				vibes_short_pulse();
				break;
			case 2:
				vibes_double_pulse();
				break;
			case 3:
				vibes_long_pulse();
				break;
		}
	   }
	GRect start01 = GRect(0, 300, 144, 168);
	GRect finish02 = GRect(0, 300, 144, 168);
	GRect finish01, start02;
	if(!isHalf){
		finish01 = GRect(0, 145, 144, 168);
		start02 = GRect(0, 145, 144, 168);
	}
	else{
		finish01 = GRect(0, 84, 144, 168);
		start02 = GRect(0, 84, 144, 168);
	}
	animate_layer(text_layer_get_layer(update_at_a_glance), &start01, &finish01, 1000, 0);
	animate_layer(text_layer_get_layer(update_at_a_glance), &start02, &finish02, 1000, animationLength);
}