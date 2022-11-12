#include "application_layer.h"
#include "uart.h"
#include <util/delay.h>

int main()
{

    AL_device_setup();
    put_str("In main\r\n");
    
    while(1);
    
}