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
//#include "driverlib/pin_map.h"
//
//#include "control_motor.h"
//
//void init_hardware(void);
//
//int main(void) {
//    init_hardware();
//    init_PWM();
//
//    int angle = 0; // Initial angle
//    float pulse = calculate_pulse_percent(angle);
//    set_motor_angle(angle);
//
//    while (1) {
//        if (GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_4) == 0x00) {
//            angle-=10;
//            if (angle <= 0) {
//                angle = 0;
//            }
//            SysCtlDelay(100000);
//        }
//
//        if (GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_0) == 0x00) {
//            angle+=10;
//            if (angle >= 180) {
//                angle = 180;
//            }
//            SysCtlDelay(100000);
//        }
//        set_motor_angle(angle);
//    }
//}
//
//
//void init_hardware() {
//    SysCtlClockSet(SYSCTL_SYSDIV_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);
//
//    // GPIO F
//    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
//    HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
//    HWREG(GPIO_PORTF_BASE + GPIO_O_CR) |= 0x01;
//    HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = 0;
//    GPIODirModeSet(GPIO_PORTF_BASE, GPIO_PIN_4|GPIO_PIN_0, GPIO_DIR_MODE_IN);
//    GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_4|GPIO_PIN_0, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
//}




// sensor
#include <stdint.h>
#include <stdbool.h>
#include "inc/tm4c123gh6pm.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/adc.h"
#include "driverlib/timer.h"
#include "driverlib/gpio.h"
#include "driverlib/uart.h"
#include "driverlib/pin_map.h"
#include "driverlib/interrupt.h"
#include <stdio.h>

#include "read_sensors.h"

//volatile uint32_t adcValues[4];
//volatile float input_voltage = 0.0, calculated_resistance = 0.0;
//char acBuffer[50];
//
//
//void calculate_resistence(void);
////void send_data(void);
//void Timer0AIntHandler(void);

int main(void)
{
    // sys clk = 40 MHz
    SysCtlClockSet(SYSCTL_SYSDIV_5 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ | SYSCTL_OSC_MAIN);

//    // Configure peripherals
//    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
//    GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_3); // PE3 as ADC input

//    // config adc
//    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
//    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
//    GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_3);
//    ADCSequenceConfigure(ADC0_BASE, 1, ADC_TRIGGER_PROCESSOR, 0);
//    ADCSequenceStepConfigure(ADC0_BASE, 1, 0, ADC_CTL_CH0);
//    ADCSequenceStepConfigure(ADC0_BASE, 1, 1, ADC_CTL_CH0);
//    ADCSequenceStepConfigure(ADC0_BASE, 1, 2, ADC_CTL_CH0);
//    ADCSequenceStepConfigure(ADC0_BASE, 1, 3, ADC_CTL_CH0 | ADC_CTL_IE | ADC_CTL_END);
//    ADCSequenceEnable(ADC0_BASE, 1);
//    ADCHardwareOversampleConfigure(ADC0_BASE, 4);
//
//    // config timer
//    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
//    TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
//    TimerLoadSet(TIMER0_BASE, TIMER_A, SysCtlClockGet() / 10 - 1); //sampling rate = 5 Hz
//    TimerIntRegister(TIMER0_BASE, TIMER_A, Timer0AIntHandler);
//    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
//    TimerEnable(TIMER0_BASE, TIMER_A);
//
//    // enable global interrupts
//    IntMasterEnable();

    config_ADC();
    IntMasterEnable();
    IntMasterEnable();

    while (1)
    {
        // main program
    }
}


//void calculate_resistence(void)
//{
//    float avg_adc_value = (adcValues[0] + adcValues[1] + adcValues[2] + adcValues[3]) / 4.0;
//    input_voltage = avg_adc_value * (3.3 / 4096.0);
//    float ref_res = 10000.0;
//    calculated_resistance = ref_res * (input_voltage / (3.3));
//}
//
//
//
//void Timer0AIntHandler(void)
//{
//    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
//
//    ADCProcessorTrigger(ADC0_BASE, 1);
//    while (!ADCIntStatus(ADC0_BASE, 1, false));
//    ADCIntClear(ADC0_BASE, 1);
//    ADCSequenceDataGet(ADC0_BASE, 1, (uint32_t *)adcValues);
//
//    // calculate and send resistance
//    calculate_resistence();
////    send_data();
//}

