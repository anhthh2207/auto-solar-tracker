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

#define PWM_FREQUENCY 50                // motor control signal = 50 Hz
#define MIN_PULSE_PERCENT 3.5f          // minimum duty cycle percentage
#define MAX_PULSE_PERCENT 13.0f         // maximum duty cycle percentage

typedef struct PWMConfig {
    uint32_t gpioPeripheral;            // GPIO peripheral
    uint32_t pwmPeripheral;             // PWM peripheral
    uint32_t gpioBase;                  // GPIO port base
    uint32_t gpioPin[2];                // GPIO pins
    uint32_t pinConfig[2];                 // Pin configuration
    uint32_t pwmGen;                    // PWM generator
    uint32_t pwmOutBit[2];                 // PWM output bit
    uint32_t pwmOut[2];
    uint32_t pwmBase;                   // PWM module base
    uint32_t pwmMode;                   // count up/down mode
    uint32_t pwdDiv;
} PWMConfig;

static const PWMConfig motorPWM_config = {
    .gpioPeripheral = SYSCTL_PERIPH_GPIOF,
    .pwmPeripheral = SYSCTL_PERIPH_PWM1,
    .gpioBase = GPIO_PORTF_BASE,
    .gpioPin = {GPIO_PIN_2, GPIO_PIN_3},
    .pinConfig = {GPIO_PF2_M1PWM6, GPIO_PF3_M1PWM7},
    .pwmGen = PWM_GEN_3,
    .pwmOutBit = {PWM_OUT_6_BIT, PWM_OUT_7_BIT},
    .pwmOut = {PWM_OUT_6, PWM_OUT_7},
    .pwmBase = PWM1_BASE,
    .pwmMode = PWM_GEN_MODE_DOWN,
    .pwdDiv = SYSCTL_PWMDIV_64
};

static uint32_t pwmClock;
static uint32_t period;


/*
 * Configure PWM generator (ouput 0, module 0, generator 0)
 *
 * Output signal pin: PB6
 * PMW clock = system clock / 64
 *
 */
void init_PWM(void) {
    SysCtlPeripheralEnable(motorPWM_config.gpioPeripheral);

    SysCtlPeripheralEnable(motorPWM_config.pwmPeripheral);
    GPIOPinTypePWM(motorPWM_config.gpioBase, motorPWM_config.gpioPin[0]|motorPWM_config.gpioPin[1]);
    GPIOPinConfigure(motorPWM_config.pinConfig[0]);
    GPIOPinConfigure(motorPWM_config.pinConfig[1]);

    SysCtlPWMClockSet(motorPWM_config.pwdDiv);

    PWMGenConfigure(motorPWM_config.pwmBase, motorPWM_config.pwmGen, motorPWM_config.pwmMode);

    pwmClock = SysCtlClockGet() / 64;
    period = (pwmClock / PWM_FREQUENCY) - 1;
    PWMGenPeriodSet(motorPWM_config.pwmBase, motorPWM_config.pwmGen, period);

    PWMOutputState(motorPWM_config.pwmBase, motorPWM_config.pwmOutBit[0], true);
    PWMOutputState(motorPWM_config.pwmBase, motorPWM_config.pwmOutBit[1], true);
    PWMGenEnable(motorPWM_config.pwmBase, motorPWM_config.pwmGen);
}


/*
 * Calculate duty percentage given target angle
 */
float calculate_pulse_percent(int angle) {
    if (angle < 0) {
        angle = 0;
    } else if (angle > 180) {
        angle = 180;
    }

    return MIN_PULSE_PERCENT + ((float)angle / 180.0f) * (MAX_PULSE_PERCENT - MIN_PULSE_PERCENT);
}


/*
 * Execute rotations given angle
 */
void set_motor0_angle(int angle) {
    uint32_t pulseWidth = (uint32_t)(calculate_pulse_percent(angle) * period / 100.0f);

    PWMPulseWidthSet(motorPWM_config.pwmBase, motorPWM_config.pwmOut[0], pulseWidth);
}

void set_motor1_angle(int angle) {
    uint32_t pulseWidth = (uint32_t)(calculate_pulse_percent(angle) * period / 100.0f);

    PWMPulseWidthSet(motorPWM_config.pwmBase, motorPWM_config.pwmOut[1], pulseWidth);
}


/*
 * Reference documents:
 * https://blog.wokwi.com/learn-servo-motor-using-wokwi-logic-analyzer/
 * https://electronics.stackexchange.com/questions/346603/driving-servo-motor-with-pwm-signal
 */




