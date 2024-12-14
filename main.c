//#include <stdio.h>
//#include <stdint.h>
//#include <stdbool.h>
//#include "inc/tm4c123gh6pm.h"
//#include "inc/hw_memmap.h"
//#include "inc/hw_types.h"
//#include "driverlib/sysctl.h"
//#include "driverlib/interrupt.h"
//#include "driverlib/gpio.h"
//#include "inc/hw_gpio.h"
//#include "driverlib/timer.h"
//#include "driverlib/pwm.h"
//#include "driverlib/pin_map.h"
//
//#define PWM_FREQUENCY 50
//volatile uint32_t ui32Load;
//volatile uint32_t ui32PWMClock;
//float percentage_PW_min = 3.5; // Pulse width in ms for 0°
//float precentage_PW_max = 13; // Pulse width in ms for 180°
//
//void setup_motor_control_signal(void);
//float calculate_percent(int angle);
//void set_motor_angle(int angle);
//
//
//void setup_motor_control_signal(void) {
//    SysCtlPWMClockSet(SYSCTL_PWMDIV_64); // set divider = 64, run the PWM clock at 625 kHz
//
//    SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM0);
//    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
//
//    GPIOPinTypePWM(GPIO_PORTB_BASE, GPIO_PIN_6); // config pin 6  as the output of the PWM
//    GPIOPinConfigure(GPIO_PB6_M0PWM0);
//
//    // Configure PWM Generator
//    PWMGenConfigure(PWM0_BASE, PWM_GEN_0, PWM_GEN_MODE_DOWN);
//
//    // Set PWM Period:
//    ui32PWMClock = SysCtlClockGet() / 64; // 625,000
//    ui32Load = (ui32PWMClock / PWM_FREQUENCY) - 1;
//    PWMGenPeriodSet(PWM0_BASE, PWM_GEN_0, ui32Load);
//
//    // Enable PWM Output
//    PWMOutputState(PWM0_BASE, PWM_OUT_0_BIT, true);
//    // Enable PWM Generator
//    PWMGenEnable(PWM0_BASE, PWM_GEN_0);
//
//}
//
//float calculate_percent(int angle) {
//    return percentage_PW_min + ((float)angle / 180.0) * (precentage_PW_max - percentage_PW_min);
//}
//
//void set_motor_angle(int angle) {
//    // Set the PWM pulse width
//    PWMPulseWidthSet(PWM0_BASE, PWM_OUT_0, calculate_percent(angle) * ui32Load / 100);
//}
//
//int main(void) {
//    volatile uint32_t ui32Load;
//    volatile uint32_t ui32PWMClock;
//    volatile float angle;
//
//    SysCtlClockSet(SYSCTL_SYSDIV_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ); // 40Mhz
////    setup_motor_control_signal();
//    SysCtlPWMClockSet(SYSCTL_PWMDIV_64); // set divider = 64, run the PWM clock at 625 kHz
//
//    SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM0);
//    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
//    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
//
//    GPIOPinTypePWM(GPIO_PORTB_BASE, GPIO_PIN_6); // config pin 6  as the output of the PWM
//    GPIOPinConfigure(GPIO_PB6_M0PWM0);
//
//    HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
//    HWREG(GPIO_PORTF_BASE + GPIO_O_CR) |= 0x01;
//    HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = 0;
//
//    GPIODirModeSet(GPIO_PORTF_BASE, GPIO_PIN_4|GPIO_PIN_0, GPIO_DIR_MODE_IN);
//    GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_4|GPIO_PIN_0, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
//
//    // Configure PWM Generator
//    PWMGenConfigure(PWM0_BASE, PWM_GEN_0, PWM_GEN_MODE_DOWN);
//
//    // Set PWM Period:
//    ui32PWMClock = SysCtlClockGet() / 64; // 625,000
//    ui32Load = (ui32PWMClock / PWM_FREQUENCY) - 1;
//    PWMGenPeriodSet(PWM0_BASE, PWM_GEN_0, ui32Load);
//
//
//    // Enable PWM Output
//    PWMOutputState(PWM0_BASE, PWM_OUT_0_BIT, true);
//    // Enable PWM Generator
//    PWMGenEnable(PWM0_BASE, PWM_GEN_0);
//
//
//    angle = 50.5;
//    PWMPulseWidthSet(PWM0_BASE, PWM_OUT_0, calculate_percent(angle) * ui32Load / 100);
////    set_motor_angle(angle);
//
//
//    while (1) {
//        if (GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_4) == 0x00) {
//            angle-=10;
//            if (angle <= 0) {
//                angle = 0;
//            }
////            angle =0;
//            SysCtlDelay(100000);
//        }
//
//        if (GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_0) == 0x00) {
//            angle+=10;
//            if (angle >= 180) {
//                angle = 180;
//            }
////            angle = 180;
//            SysCtlDelay(100000);
//        }
//        PWMPulseWidthSet(PWM0_BASE, PWM_OUT_0, calculate_percent(angle) * ui32Load / 100);
////        set_motor_angle(angle);
//    }
//
//}



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
#include "driverlib/timer.h"
#include "driverlib/pwm.h"
#include "driverlib/pin_map.h"

#include "control_motor.h"
//
//#define PWM_FREQUENCY 50
//#define MIN_PULSE_PERCENT 3.5f
//#define MAX_PULSE_PERCENT 13.0f
//
//uint32_t pwmClock;
//uint32_t period;
//
//void initializeHardware(void);
//void configure_PWM(void);
//float calculate_percent(int angle);
//void set_motor_angle(int angle);

int main(void) {
    SysCtlClockSet(SYSCTL_SYSDIV_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
    HWREG(GPIO_PORTF_BASE + GPIO_O_CR) |= 0x01;
    HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = 0;
    GPIODirModeSet(GPIO_PORTF_BASE, GPIO_PIN_4|GPIO_PIN_0, GPIO_DIR_MODE_IN);
    GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_4|GPIO_PIN_0, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

    SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    GPIOPinTypePWM(GPIO_PORTB_BASE, GPIO_PIN_6);
    GPIOPinConfigure(GPIO_PB6_M0PWM0);
    configure_PWM();


    int angle = 10; // Initial angle
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

//void initializeHardware(void) {
//    SysCtlClockSet(SYSCTL_SYSDIV_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);
//
//    SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM0);
//    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
//    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
//
//
//    GPIOPinTypePWM(GPIO_PORTB_BASE, GPIO_PIN_6);
//    GPIOPinConfigure(GPIO_PB6_M0PWM0);
//}
//
//void configure_PWM(void) {
//    SysCtlPWMClockSet(SYSCTL_PWMDIV_64);
//
//    PWMGenConfigure(PWM0_BASE, PWM_GEN_0, PWM_GEN_MODE_DOWN);
//
//    pwmClock = SysCtlClockGet() / 64;
//    period = (pwmClock / PWM_FREQUENCY) - 1;
//    PWMGenPeriodSet(PWM0_BASE, PWM_GEN_0, period);
//
//    PWMOutputState(PWM0_BASE, PWM_OUT_0_BIT, true);
//    PWMGenEnable(PWM0_BASE, PWM_GEN_0);
//}
//
//float calculate_percent(int angle) {
//    if (angle < 0) {
//        angle = 0;
//    } else if (angle > 180) {
//        angle = 180;
//    }
//
//    return MIN_PULSE_PERCENT + ((float)angle / 180.0f) * (MAX_PULSE_PERCENT - MIN_PULSE_PERCENT);
//}
//
//void set_motor_angle(int angle) {
//    uint32_t pulseWidth = (uint32_t)(calculate_percent(angle) * period / 100.0f);
//
//    PWMPulseWidthSet(PWM0_BASE, PWM_OUT_0, pulseWidth);
//}
