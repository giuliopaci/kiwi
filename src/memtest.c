#include "kiwi.h"
#include "io.h"

// Run the thing 2k times so we can watch memory usage externally.

int main() {
  int i;
  for(i = 0; i < 2000; i++) {
    Kw* k = kw_init();
    file_get_contents(input_buffer, "spec/fixtures/tables");
    kw_parse(k);
    printf("%d\n", i);
    kw_cleanup(k);
  }
  return 0;
}
