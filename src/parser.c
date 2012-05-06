#include "kiwi.h"
#include "io.h"

int main(int argc, char **argv) {

   Kw* k = kw_init();
   //file_get_contents(input, "spec/fixtures/cnn.com");
   stdin_get_contents(input_buffer);
   if(argc > 0) {
     set_base_url(argv[1]);
   }
   if(argc > 1) {
     set_image_base_url(argv[2]);
   }
   kw_parse(k);
   printf("%s", bdata(output_buffer));
   kw_cleanup(k);
   return 0;
 }
