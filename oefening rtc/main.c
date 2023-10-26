#include "cyhal.h"
#include "cybsp.h"
#include "cy_retarget_io.h"
#include <time.h>
#include <stdio.h>

#define TM_YEAR_BASE (1900u)

int main(void)
{

	cybsp_init();
	 __enable_irq();
    cy_retarget_io_init(CYBSP_DEBUG_UART_TX, CYBSP_DEBUG_UART_RX, CY_RETARGET_IO_BAUDRATE);

    cy_rslt_t result;
    cyhal_rtc_t my_rtc;

    char buffer[80];

    cyhal_gpio_t buttonPin =(P0_4);
    cyhal_gpio_init(buttonPin, CYHAL_GPIO_DIR_INPUT, CYHAL_GPIO_DRIVE_NONE, CYHAL_GPIO_IRQ_NONE);

    int mday = 3, month = 3, year = 2020; // Day of month, month and year
    int hours = 8, minutes = 10, seconds = 10; // Hours, minutes and seconds
    int wday = 3; // Days from Sunday. Sunday is 0, Monday is 1 and so on.
    int dst = 0; // Daylight Savings. 0 - Disabled, 1 - Enabled

    struct tm new_date_time =
    {
        .tm_sec = seconds,
        .tm_min = minutes,
        .tm_hour = hours,
        .tm_mday = mday,
        .tm_mon = month - 1,
        .tm_year = year - TM_YEAR_BASE,
        .tm_wday = wday,
        .tm_isdst = dst
    };
    struct tm current_date_time = {0};
    // Initialize the RTC
    cyhal_rtc_init(&my_rtc);
    // Set the RTC time
    cyhal_rtc_write(&my_rtc, &new_date_time);
    for (;;)
    {
        uint8_t buttonStatus = cyhal_gpio_read(buttonPin);

        if (buttonStatus == 0) // Assuming the button is active low
        {
            // Get the current time and date from the RTC peripheral
            result = cyhal_rtc_read(&my_rtc, &current_date_time);
            if (CY_RSLT_SUCCESS == result)
            {
                strftime(buffer, sizeof(buffer), "%c", &current_date_time);
                printf("Button pressed. Current Date and Time: %s\n\r", buffer);
            }
        }
        else
        {
        	printf("Tick!\n\r"); // Print "Tick!" every second
        }


        cyhal_system_delay_ms(1000);
    }
}
