extern "C" {
#include <pebble.h>

#include <math.h>

#ifdef USE_FIXED_POINT
  #include "math-sll.h"
  static __inline__ sll dbl(double x) {
    return dbl2sll(x);
  }
#else
  #define CONST_PI		3.14159265358979323846
  //#define dbl(a) (a)
#endif
}

//Math bugs!!!!
//cannot do cos(-rotation), need cos(dbl(-1.0) * rotation)
//should re-add __aeabi_dneg (OTI_neg?) to compiler
//cannot do hard ints, treats them as SLLs!!! for initializers for SLLs


class SLL {
  public:
    sll value;
    inline SLL(){ this->value = 0; }
    inline ~SLL(){}

    inline SLL(int a){ this->value = int2sll(a); }
    inline SLL(sll a){ this->value = a; }
    //inline SLL(double a){ this->value = dbl2sll(a); }
    
    inline SLL operator+(const SLL &b){ return SLL(slladd(this->value,b.value)); }
    inline SLL operator-(const SLL &b){ return SLL(sllsub(this->value,b.value)); }
    inline SLL operator*(const SLL &b){ return SLL(sllmul(this->value,b.value)); }
    inline SLL operator/(const SLL &b){ return SLL(slldiv(this->value,b.value)); }

    inline SLL operator*(const sll &b){ return SLL(sllmul(this->value,b)); }
    inline SLL operator*(const int &b){ return SLL(sllmul(this->value,int2sll(b))); }

    inline SLL operator-(const int &b){ return SLL(sllsub(this->value,int2sll(b))); }
    inline SLL operator-(const sll &b){ return SLL(sllsub(this->value,b)); }
    //inline SLL operator-(const double &b){ return SLL(sllsub(this->value,dbl2sll(b))); }
    inline int operator=(SLL a){ return sll2int(a.value); }

    inline operator int(){ return this->value; }
    
};
    

SLL cos(SLL a){ return SLL(sllcos(a.value)); }
SLL sin(SLL a){ return SLL(sllsin(a.value)); }
SLL tan(SLL a){ return SLL(slltan(a.value)); }

extern "C" {

//#define SCALE          9     // determines how quickly branches shrink (higher value means faster shrinking)
//#define BRANCHES       8    // number of branches
//#define INITIAL_LENGTH 18    // length of first branch

SLL lean_left;
SLL lean_right;


SLL rand_fl(){
  int rand_max_i = RAND_MAX;
  SLL rand_SLL = rand();
  SLL rand_max_d = rand_max_i;
  return (rand_SLL / rand_max_d);
}
   
void draw_tree(GContext* ctx, SLL offsetx, SLL offsety,
    SLL directionx, SLL directiony, SLL size,
    SLL rotation, int depth) {
  // determines how slowly the angle between branches shrinks 
  // (higher value means slower shrinking)
  SLL rotation_scale = dbl(0.85);
  SLL SCALE = dbl(9.0);


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
    SLL dx1 = offsetx + directionx * size;
    SLL dy1 = offsety + directiony * size;

    SLL dx2 = directionx * cos(rotation) + directiony * sin(rotation);
    SLL dy2 = dbl(-1.0) * directionx * sin(rotation) + directiony * cos(rotation);

    SLL next_size = size * rand_fl() / SCALE + size * (SCALE - dbl(1.0)) / SCALE;
    SLL next_rotation = rotation * rotation_scale;
    // draw left branch
    draw_tree(ctx,
        dx1, dy1,
        dx2, dy2,
        next_size,
        next_rotation * lean_left,
        depth - 1);

    SLL neg_rotation = dbl(-1.0) * rotation;
    SLL dx3 = directionx * cos(neg_rotation) + directiony * sin(neg_rotation);
    SLL dy3 = dbl(-1.0) * directionx * sin(neg_rotation) + directiony * cos(neg_rotation);

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
  SLL BRANCHES = dbl(8.0);
  SLL INITIAL_LENGTH = dbl(20.0);
  SLL pi_8 = dbl(0.39269908169);
  SLL width = dbl(72.0);
  SLL height = dbl(158.0);
  SLL negone = dbl(-1.0);
  SLL dzero = dbl(0.0);
  
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
       window_load,
       NULL, NULL,
      window_unload,
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

}
