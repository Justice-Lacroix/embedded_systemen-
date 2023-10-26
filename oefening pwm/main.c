/*We lezen een waarde uit die we ingeven op de terminal, dit is de knipperfrequentie van de led.
 * Deze frequentie sturen we naar de led en de waarde printen we ook uit.
 * good to know: de scanf() is blokkend!
 */

#include "cyhal.h"
#include "cybsp.h"
#include "cy_retarget_io.h"

int main(void)
{
    int frequentie = 1;

	cybsp_init();
    __enable_irq();

    cyhal_pwm_t pwm_obj;

	// Initialize PWM on the supplied pin and assign a new clock
	cyhal_pwm_init(&pwm_obj, P13_7, NULL);

	// Set a duty cycle of 50% and frequency of 1Hz
	cyhal_pwm_set_duty_cycle(&pwm_obj, 50, frequentie);

	// Start the PWM output
	cyhal_pwm_start(&pwm_obj);

    cy_retarget_io_init(CYBSP_DEBUG_UART_TX, CYBSP_DEBUG_UART_RX, 115200);

    for (;;)
    {
    	printf("Frequentie = %d\n\r", frequentie);

    	scanf("%d", &frequentie);

    	cyhal_pwm_set_duty_cycle(&pwm_obj, 50, frequentie);
    }
}
