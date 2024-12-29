#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include "inc/tm4c123gh6pm.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"
#include "inc/hw_gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/adc.h"
#include "driverlib/timer.h"
#include "driverlib/uart.h"
#include "driverlib/systick.h"
#include "driverlib/pwm.h"

/////////////////////////////
#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include <stdio.h>

#include "inc/tm4c123gh6pm.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "inc/hw_timer.h"

#include "driverlib/sysctl.h"
#include "driverlib/pin_map.h"
#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"
#include "driverlib/adc.h"
#include "driverlib/pwm.h"
#include "driverlib/uart.h"
#include "driverlib/debug.h"
#include "driverlib/systick.h"
////////////////////////////////////

#include "control_motor.h"
#include "read_sensors.h"
#include "calculate_angle.h"

void SysTickIntHandler(void);
void init_hardware(void);
void setup_UART(void);
void send_message(void);

char acBuffer[100];

volatile int angle0 = 90; // Initial angle
volatile int angle1 = 90;
volatile float ldr0 = 0;
volatile float ldr1 = 0;
volatile float ldr2 = 0;
volatile float ldr3 = 0;
volatile float pannel_vol = 0;


int main(void)
{
    init_hardware();
    init_PWM();
    config_ADC();
//    setup_UART();
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200, (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));

    // initialize position
    float pulse = calculate_pulse_percent(angle0);
    set_motor0_angle(angle0);
    pulse = calculate_pulse_percent(angle1);
    set_motor0_angle(angle1);

    SysTickPeriodSet((SysCtlClockGet()/ get_sampling_rate()) - 1);
    SysTickIntEnable();
    IntMasterEnable();
    SysTickEnable();

    while (1) {
        // main program
        // set_motor_angle(angle);
    }
}

void SysTickIntHandler(void)
{
    update_sensors_data();
    ldr0 = get_ldr_data()[0];
    ldr1 = get_ldr_data()[1];
    ldr2 = get_ldr_data()[2];
    ldr3 = get_ldr_data()[3];
    pannel_vol = get_pannel_voltage();

    angle0 = update_new_angle(angle0, ldr0, ldr1);
    set_motor0_angle(angle0);

    angle1 = update_new_angle(angle1, ldr2, ldr3);
    set_motor1_angle(angle1);

    send_message();
}


void init_hardware() {
    SysCtlClockSet(SYSCTL_SYSDIV_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);
}


void setup_UART(void) {
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200, (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));
}

void send_message(void) {
//    uint32_t noBytes = sprintf(acBuffer, "LDR0: %.2f\t LDR1: %.2f\t Angle0: %d\t LDR3: %.2f\t LDR4: %.2f\t Angle1: %d\t SolarVol: %.4f\n", ldr0, ldr1, angle0, ldr2, ldr3, angle1, pannel_vol);
//    uint32_t noBytes = sprintf(acBuffer, "V202100405 - %d\n", (uint32_t)(ldr0));
//    uint32_t idx;
//    for (idx = 0; idx < noBytes; idx++)
//    {
//        UARTCharPut(UART0_BASE, acBuffer[idx]);
//    }

    const char *studentID = "V202100405";
    uint32_t idx = 0;

    while (studentID[idx] != '\0')
    {
        UARTCharPut(UART0_BASE, studentID[idx]);
        idx++;
    }

}
