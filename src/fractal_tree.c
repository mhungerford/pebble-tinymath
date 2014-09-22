#include <pebble.h>

#include <math.h>

#ifdef USE_FIXED_POINT
  #include "math-sll.h"
  static __inline__ double dbl(double x) {
    sll sval = dbl2sll(x);
    return *(double*)&sval;
  }
#else
  #define CONST_PI		3.14159265358979323846
  #define dbl(a) (a)
#endif

//Math bugs!!!!
//cannot do cos(-rotation), need cos(dbl(-1.0) * rotation)
//should re-add __aeabi_dneg (OTI_neg?) to compiler
//cannot do hard ints, treats them as doubles!!! for initializers for doubles



//#define SCALE          9     // determines how quickly branches shrink (higher value means faster shrinking)
//#define BRANCHES       8    // number of branches
//#define INITIAL_LENGTH 18    // length of first branch

double lean_left;
double lean_right;


double rand_fl(){
  int rand_max_i = RAND_MAX;
  double rand_double = rand();
  double rand_max_d = rand_max_i;
  return (rand_double / rand_max_d);
}
   
void draw_tree(GContext* ctx, double offsetx, double offsety,
    double directionx, double directiony, double size,
    double rotation, int depth) {
  // determines how slowly the angle between branches shrinks 
  // (higher value means slower shrinking)
  double rotation_scale = dbl(0.85);
  double SCALE = dbl(9.0);


  int x1 = offsetx;
  int y1 = offsety;
  int x2 = offsetx + directionx * size;
  int y2 = offsety + directiony * size;

  // Protect against drawing offscreen
  if ( 0 < x1 && x1 < 144 &&
    0 < y1 && y1 < 168 &&
    0 < x2 && x2 < 144 &&
    0 < y2 && y2 < 168) {
    graphics_draw_line(ctx, (GPoint){x1, y1}, (GPoint){x2, y2});
  }

  if (depth > 0){
    double dx1 = offsetx + directionx * size;
    double dy1 = offsety + directiony * size;

    double dx2 = directionx * cos(rotation) + directiony * sin(rotation);
    double dy2 = dbl(-1.0) * directionx * sin(rotation) + directiony * cos(rotation);

    double next_size = size * rand_fl() / SCALE + size * (SCALE - dbl(1.0)) / SCALE;
    double next_rotation = rotation * rotation_scale;
    // draw left branch
    draw_tree(ctx,
        dx1, dy1,
        dx2, dy2,
        next_size,
        next_rotation * lean_left,
        depth - 1);

    double neg_rotation = dbl(-1.0) * rotation;
    double dx3 = directionx * cos(neg_rotation) + directiony * sin(neg_rotation);
    double dy3 = dbl(-1.0) * directionx * sin(neg_rotation) + directiony * cos(neg_rotation);

    // draw right branch
    draw_tree(ctx,
        dx1, dy1,
        dx3, dy3,
        next_size,
        next_rotation * lean_right,
        depth - 1);
  }
}

static void update_display(Layer* layer, GContext *ctx) {
  double BRANCHES = dbl(8.0);
  double INITIAL_LENGTH = dbl(20.0);
  double pi_8 = dbl(0.39269908169);
  double width = dbl(72.0);
  double height = dbl(158.0);
  double negone = dbl(-1.0);
  double dzero = dbl(0.0);
  
  time_t start_time;
  uint16_t start_time_ms; 
  time_ms(&start_time, &start_time_ms);

  lean_left = dbl(1.12);
  lean_right = dbl(1.12);

  draw_tree(ctx,
      width, //144 / 2,
      height,//168 - 10,
      dzero, negone,
      INITIAL_LENGTH,
      pi_8,
      BRANCHES);
  time_t end_time;
  uint16_t end_time_ms; 
  time_ms(&end_time, &end_time_ms);
  printf("took %ld ms", 
      (end_time * 1000 + end_time_ms) -
      (start_time * 1000 + start_time_ms));
}

static Window *window;
static Layer *render_layer;

static void register_timer(void* data) {
  app_timer_register(100, register_timer, data);
  layer_mark_dirty(render_layer);
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  render_layer = layer_create(bounds);
  layer_set_update_proc(render_layer, update_display);
  layer_add_child(window_layer, render_layer);
  register_timer(NULL);
}

static void window_unload(Window *window) {
}

static void init(void) {
  window = window_create();
  window_set_fullscreen(window, true);
  window_set_window_handlers(window, (WindowHandlers) {
      .load = window_load,
      .unload = window_unload,
      });
  const bool animated = true;
  window_stack_push(window, animated);
}

static void deinit(void) {
  window_destroy(window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
