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

#include "control_motor.h"

void init_hardware(void);

int main(void) {
    init_hardware();
    init_PWM();

    int angle = 0; // Initial angle
    float pulse = calculate_pulse_percent(angle);
    set_motor_angle(angle);

    while (1) {
        if (GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_4) == 0x00) {
            angle-=10;
            if (angle <= 0) {
                angle = 0;
            }
            SysCtlDelay(100000);
        }

        if (GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_0) == 0x00) {
            angle+=10;
            if (angle >= 180) {
                angle = 180;
            }
            SysCtlDelay(100000);
        }
        set_motor_angle(angle);
    }
}


void init_hardware() {
    SysCtlClockSet(SYSCTL_SYSDIV_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

    // GPIO F
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
    HWREG(GPIO_PORTF_BASE + GPIO_O_CR) |= 0x01;
    HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = 0;
    GPIODirModeSet(GPIO_PORTF_BASE, GPIO_PIN_4|GPIO_PIN_0, GPIO_DIR_MODE_IN);
    GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_4|GPIO_PIN_0, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
}
