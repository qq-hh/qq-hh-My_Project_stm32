#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "usart.h"
#include "ws2812.h"
 
int8_t i;
void Delay(u32 count)
{
    u32 i = 0;
    for(; i < count; i++);
}


int main(void)
{
    uart_init(115200);
    delay_init();
    ws281x_init();
    while(1)
    {
        ws281x_colorWipe(ws281x_color(255, 0, 0), 50); // Red
        ws281x_colorWipe(ws281x_color(0, 255, 0), 50); // Green
			  ws281x_colorWipe(ws281x_color(200, 255, 0), 50); //»ÆÉ«
		  	ws281x_colorWipe(ws281x_color(100, 255, 50), 50); //±ùÀ¶
			  ws281x_colorWipe(ws281x_color(155, 255, 0), 50); //ÄÛÂÌÉ«
			
//        for(i = 0; i < PIXEL_NUM; ++i)
//        {
//            ws281x_setPixelColor(i, ws281x_color(255, 250, 0));
//            ws281x_show();
//            delay_ms(5);
//        }


    }
}

