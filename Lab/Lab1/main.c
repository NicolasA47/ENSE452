#include "util.h"

int main(void){
	clockInit();
  led_IO_init();
	
	while(1){
		led_flash();
	}
	
}
