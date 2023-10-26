#include "cyhal.h"
#include "cybsp.h"
#include "cy_retarget_io.h"

/* Delay of 1000ms between commands */
#define CMD_TO_CMD_DELAY        (1000UL)

/* Packet positions */
#define PACKET_SOP_POS          (0UL)
#define PACKET_CMD_POS          (1UL)
#define PACKET_EOP_POS          (2UL)

/* Start and end of packet markers */
#define PACKET_SOP              (0x01UL)
#define PACKET_EOP              (0x17UL)

/* I2C slave address to communicate with */
#define I2C_SLAVE_ADDR          (0x24UL)

/* I2C bus frequency */
#define I2C_FREQ                (400000UL)

/* Command valid status */
#define STATUS_CMD_DONE         (0x00UL)

/* Packet size */
#define PACKET_SIZE             (3UL)

void handle_error(uint32_t status)
{
    if (status != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }
}

int main(void)
{
    cy_rslt_t result;
    cyhal_i2c_t mI2C;
    cyhal_i2c_cfg_t mI2C_cfg;
    uint8_t cmd = CYBSP_LED_STATE_ON;
    uint8_t buffer[PACKET_SIZE];

    /* Initialize the device and board peripherals */
    result = cybsp_init();
    /* Board init failed. Stop program execution */
    handle_error(result);

    /* Initialize the retarget-io */
    result = cy_retarget_io_init(CYBSP_DEBUG_UART_TX, CYBSP_DEBUG_UART_RX,
                                  CY_RETARGET_IO_BAUDRATE);
    /* Retarget-io init failed. Stop program execution */
    handle_error(result);

    /* \x1b[2J\x1b[;H - ANSI ESC sequence for clear screen */
    printf("\x1b[2J\x1b[;H");

    printf("****************** "
           "HAL: I2C Master "
           "****************** \r\n\n");

    /* I2C Master configuration settings */
    printf(">> Configuring I2C Master..... ");
    mI2C_cfg.is_slave = false;
    mI2C_cfg.address = 0;
    mI2C_cfg.frequencyhal_hz = I2C_FREQ;

    /* Init I2C master */
    result = cyhal_i2c_init(&mI2C, CYBSP_I2C_SDA, CYBSP_I2C_SCL, NULL);
    /* I2C master init failed. Stop program execution */
    handle_error(result);

    /* Configure I2C Master */
    result = cyhal_i2c_configure(&mI2C, &mI2C_cfg);
    /* I2C master configuration failed. Stop program execution */
    handle_error(result);

    printf("Done\r\n\n");

    /* Enable interrupts */
    __enable_irq();

    for (;;)
    {
        /* Create packet to be sent to slave */
        buffer[PACKET_SOP_POS] = PACKET_SOP;
        buffer[PACKET_EOP_POS] = PACKET_EOP;
        buffer[PACKET_CMD_POS] = cmd;

        /* Send packet with command to the slave */
        if (CY_RSLT_SUCCESS == cyhal_i2c_master_write(&mI2C, I2C_SLAVE_ADDR,
                                                  buffer, PACKET_SIZE, 0, true))
        {
            /* Read response packet from the slave */
            if (CY_RSLT_SUCCESS == cyhal_i2c_master_read(&mI2C, I2C_SLAVE_ADDR,
                                                 buffer, PACKET_SIZE, 0, true))
            {
                /* Check packet structure and status */
                if ((PACKET_SOP == buffer[PACKET_SOP_POS]) &&
                   (PACKET_EOP == buffer[PACKET_EOP_POS]) &&
                   (STATUS_CMD_DONE == buffer[PACKET_CMD_POS]))
                {
                    /* Next command to be written */
                    cmd = (cmd == CYBSP_LED_STATE_ON) ?
                           CYBSP_LED_STATE_OFF : CYBSP_LED_STATE_ON;
                }
                else
                {
                    handle_error(1);
                }
            }
            /* Give delay between commands */
            cyhal_system_delay_ms(CMD_TO_CMD_DELAY);
        }
    }
}
