#if 0
#include "cloggen.h"

int main(void) {
  CLOG_INIT();
  ClogString str = clog_string_make("Hello, world!\n");
  CLOG_STRING_FPRINT(str);
  return EXIT_SUCCESS;
}
#endif

#include <stdio.h>
#include <math.h>

typedef struct { double r; } Circle;
typedef struct { double w; double h; } Rect;
typedef struct { double w; } Square;

double area_circ(Circle c)   { return M_PI * c.r * c.r; }
double area_rect(Rect r)     { return r.w * r.h; }
double area_square(Square s) { return s.w * s.w; }

#define AREA_PAIR(TYPE, FN) TYPE: FN

#define AREA_PAIR_LIST           \
  AREA_PAIR(Circle, area_circ),  \
  AREA_PAIR(Rect, area_rect),    \
  AREA_PAIR(Square, area_square)

#define AREA(SHAPE) _Generic((SHAPE), AREA_PAIR_LIST)(SHAPE)

int main(void) {

  Circle circ   = { 1.5 };
  Rect   rect   = { 1.5, 2.5 };
  Square square = { 1.5 };
  printf("Circle area: %lf\n", AREA(circ));
  printf("Rect area: %lf\n",   AREA(rect));
  printf("Square area: %lf\n", AREA(square));
  return 0;
}
