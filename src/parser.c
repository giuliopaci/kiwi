#include "kiwi.h"
#include "io.h"

int main(int argc, char **argv) {

   Kw* k = kw_init();
   //file_get_contents(input, "spec/fixtures/cnn.com");
   stdin_get_contents(kw_get_input_buffer(k));
   if(argc > 0) {
	   kw_set_base_url(k, argv[1]);
   }
   if(argc > 1) {
	   kw_set_image_base_url(k, argv[2]);
   }
   kw_parse(k);
   printf("%s", bdata(kw_get_output_buffer(k)));
   kw_cleanup(k);
   return 0;
 }
