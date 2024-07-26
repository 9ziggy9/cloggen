#include "cloggen.h"

int main(void) {
  CLOG_INIT();
  ClogString str = clog_string_make("Hello, world!\n");
  CLOG_STRING_FPRINT(str);
  return EXIT_SUCCESS;
}
