// Because gcc needs specific types for builtin softfloat, use double 
// implementations to match size of sll type


#ifdef USE_FIXED_POINT

#pragma message "Using FIXED_POINT softfloat wrapper"

//#pragma GCC optimize ("O3")
#ifdef __arm__
  #pragma message "__arm__ enabled"
#endif
#include "math-sll.h"

double __aeabi_dsub(double x, double y) {
  sll a = *(sll*)&x;
  sll b = *(sll*)&y;
  sll result = sllsub(a, b);
  return *(double*)&result;
}

double __aeabi_dadd(double x, double y) {
  sll a = *(sll*)&x;
  sll b = *(sll*)&y;
  sll result = slladd(a, b);
  return *(double*)&result;
}

double __aeabi_dmul(double x, double y) {
  sll a = *(sll*)&x;
  sll b = *(sll*)&y;
  sll result = sllmul(a, b);
  return *(double*)&result;
}

double __aeabi_ddiv(double x, double y) {
  sll a = *(sll*)&x;
  sll b = *(sll*)&y;
  sll result = slldiv(a, b);
  return *(double*)&result;
}

double __aeabi_i2d(int x) {
  sll result = int2sll(x);
  return *(double*)&result;
}

//Need to add this to optabs.h for compiler to do -dval
double __aeabi_dneg(double x) {
  sll a = *(sll*)&x;
  sll result = sllneg(a);
  return *(double*)&result;
}

int __aeabi_d2iz(double x) {
  sll a = *(sll*)&x;
  return sll2int(a);
}

int __aeabi_dcmpgt(double x, double y) {
  sll a = *(sll*)&x;
  sll b = *(sll*)&y;
  return (a > b) ? 1 : 0;
}

int __aeabi_dcmpge(double x, double y) {
  sll a = *(sll*)&x;
  sll b = *(sll*)&y;
  return (a >= b) ? 1 : 0;
}

int __aeabi_dcmplt(double x, double y) {
  sll a = *(sll*)&x;
  sll b = *(sll*)&y;
  return (a < b) ? 1 : 0;
}

int __aeabi_dcmple(double x, double y) {
  sll a = *(sll*)&x;
  sll b = *(sll*)&y;
  return (a <= b) ? 1 : 0;
}

// Trig

double cos(double x) {
  sll a = *(sll*)&x;
  sll result = sllcos(a);
  return *(double*)&result;
}

double sin(double x) {
  sll a = *(sll*)&x;
  sll result = sllsin(a);
  return *(double*)&result;
}

double tan(double x) {
  sll a = *(sll*)&x;
  sll result = slltan(a);
  return *(double*)&result;
}

#endif
