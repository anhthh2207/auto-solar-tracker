#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
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

#include "control_motor.h"
#include "read_sensors.h"
#include "calculate_angle.h"

void SysTickIntHandler(void);
void init_hardware(void);
void setup_UART(void);
void send_message(void);

char acBuffer[200];

int angle = 90; // Initial angle
float ldr0;
float ldr1;
volatile float pannel_vol = 0;


int main(void)
{
    init_hardware();
    init_PWM();
    setup_UART();

    config_ADC();

    float pulse = calculate_pulse_percent(angle);
    set_motor_angle(angle);
    
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
    pannel_vol = get_pannel_voltage();

    angle = update_new_angle(angle, ldr0, ldr1);
    set_motor_angle(angle);

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
    uint32_t noBytes = sprintf(acBuffer, "ldr0: %.2f\t ldr1: %.2f\t Angle: %d\t SolarVol: %.4f\n", ldr0, ldr1, angle, pannel_vol);
//    uint32_t noBytes = sprintf(acBuffer, "V202100405 - %d\n", (uint32_t)(ldr0));
    uint32_t idx;
    for (idx = 0; idx < noBytes; idx++)
    {
        UARTCharPut(UART0_BASE, acBuffer[idx]);
    }

//    const char *studentID = "V202100405";
//    uint32_t idx = 0;
//
//    while (studentID[idx] != '\0')
//    {
//        UARTCharPut(UART0_BASE, studentID[idx]);
//        idx++;
//    }

}
