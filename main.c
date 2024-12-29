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

int angle = 90; // Initial angle
float ldr1;
float ldr2;
float ldr3;
float ldr4;
float pannel_vol;


int main(void)
{
    init_hardware();
    init_PWM();

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
    ldr1 = get_ldr_data()[0];
    ldr2 = get_ldr_data()[1];
    ldr3 = get_ldr_data()[2];
    ldr4 = get_ldr_data()[3];
    pannel_vol = get_pannel_voltage();

    angle = update_new_angle(angle, ldr1, ldr2);
    set_motor_angle(angle);
}


void init_hardware() {
    SysCtlClockSet(SYSCTL_SYSDIV_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);
}

