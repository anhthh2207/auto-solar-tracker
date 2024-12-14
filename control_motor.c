#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "inc/tm4c123gh6pm.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "inc/hw_gpio.h"
#include "driverlib/pwm.h"
#include "driverlib/pin_map.h"

#include "control_motor.h"

#define PWM_FREQUENCY 50
#define MIN_PULSE_PERCENT 3.5f
#define MAX_PULSE_PERCENT 13.0f

uint32_t pwmClock;
uint32_t period;

void configure_PWM(void) {
    SysCtlPWMClockSet(SYSCTL_PWMDIV_64);

    PWMGenConfigure(PWM0_BASE, PWM_GEN_0, PWM_GEN_MODE_DOWN);

    pwmClock = SysCtlClockGet() / 64;
    period = (pwmClock / PWM_FREQUENCY) - 1;
    PWMGenPeriodSet(PWM0_BASE, PWM_GEN_0, period);

    PWMOutputState(PWM0_BASE, PWM_OUT_0_BIT, true);
    PWMGenEnable(PWM0_BASE, PWM_GEN_0);
}

float calculate_pulse_percent(int angle) {
    if (angle < 0) {
        angle = 0;
    } else if (angle > 180) {
        angle = 180;
    }

    return MIN_PULSE_PERCENT + ((float)angle / 180.0f) * (MAX_PULSE_PERCENT - MIN_PULSE_PERCENT);
}

void set_motor_angle(int angle) {
    uint32_t pulseWidth = (uint32_t)(calculate_pulse_percent(angle) * period / 100.0f);

    PWMPulseWidthSet(PWM0_BASE, PWM_OUT_0, pulseWidth);
}


