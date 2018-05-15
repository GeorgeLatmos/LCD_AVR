/*
 * LCD_Final.c
 *
 * Created: 13/4/2018 9:54:38 πμ
 * Author : Nick
 */ 
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include "rtc.h"
#include "lcd.h"
#include "dht22.h"
#include "bit_manipulations.h"

#define SET_TIME_ON_START 1

#define LED PD4

int main() 
{
    rtc_t rtc;
	
	BIT_SET(DDRD,LED);
	BIT_SET(PORTD,LED);
	
	lcd_init(LCD_DISP_ON_CURSOR);		
		
	lcd_clrscr();             /* clear screen of lcd */
	lcd_home();               /* bring cursor to 0,0 */
	lcd_puts("    Welcome");        /* type something random */
	lcd_gotoxy(0,1);          /* go to 2nd row 1st col */
	lcd_puts("  Latmos Ltd");  /* type something random */
	lcd_command(LCD_DISP_ON);
	_delay_ms(3000);            /* wait 50ms */
	
    /* Connect SCL->PC5, SDA->PC4
		   Register values are stored in hex format
		   Upper nibble: X
			 Lower nibble: Y
			 Register: XY
			 e.g rtc.min=48 => X = 4, Y = 8 */    
    
	RTC_Init();
		
		/* Set time only once and then disable the following code*/
	#if SET_TIME_ON_START
				
		rtc.hour = 0x20; //  10:40:20 am
		rtc.min =  0x22;
		rtc.sec =  0x00;

		rtc.date = 0x15; //1st Jan 2016
		rtc.month = 0x05;
		rtc.year = 0x18;
		rtc.weekDay = 2; // Friday: 5th day of week considering monday as first day.
		/*##### Set the time and Date only once. Once the Time and Date is set, comment these lines
					and reflash the code. Else the time will be set every time the controller is reset*/
		RTC_SetDateTime(&rtc);  //  10:40:20 am, 1st Jan 2016
	#endif
		
    while(1)
    {
		int temperature = 0, humidity = 0;
				
        RTC_GetDateTime(&rtc);
				
		dhtxxconvert(DHTXX_DHT22, &DHT_PORT, &DHT_DDR, &DHT_PIN, ( 1 << DHT_DATA_PIN ) );  //	Start conversion
		_delay_ms(1000);
		dhtxxread(DHTXX_DHT22, &DHT_PORT, &DHT_DDR, &DHT_PIN, ( 1 << DHT_DATA_PIN ), &temperature, &humidity ); // Get values
		temperature /= 10;
		humidity /= 10;
				
		char sec[3];
		char min[3];
		char hour[3];
		char date[3];
		char month[3];
		char year[5];
		sprintf(sec,"%02x", (uint8_t)rtc.sec);
		sprintf(min,"%02x", (uint8_t)rtc.min);
		sprintf(hour,"%02x", (uint8_t)rtc.hour);
		sprintf(date,"%02x", (uint8_t)rtc.date);
		sprintf(month,"%02x", (uint8_t)rtc.month);
		sprintf(year,"20%02x", (uint8_t)rtc.year);
				
		char temp_hum[20];
		char temp[4];
		char hum[4];
		sprintf(temp, "%d", temperature);
		sprintf(hum, "%d", humidity);
		sprintf(temp_hum, "  T=%sC  H=%s%%", temp, hum);
				
		char date_time[17];
		sprintf(date_time, "%s/%s/%s %s:%s", date, month, year, hour, min);
		lcd_clrscr();             /* clear screen of lcd */
		lcd_home();               /* bring cursor to 0,0 */
		lcd_puts(date_time);			/* datetime */
		lcd_gotoxy(0,1);          /* go to 2nd row 1st col */
		lcd_puts(temp_hum);				/* print Temperature and Humidity*/
		lcd_command(LCD_DISP_ON);
				
		_delay_ms(1000);            /* wait 50ms */
    }
    return (0);
}

