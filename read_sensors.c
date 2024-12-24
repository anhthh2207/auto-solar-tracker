#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "inc/tm4c123gh6pm.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/adc.h"
#include "driverlib/timer.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/interrupt.h"
#include "read_sensors.h"

typedef struct {
    uint32_t gpio_peripheral ;    // GPIO peripheral
    uint32_t adc_peripheral;
    int sequencer;
    uint32_t adc_base;          // Base address of the ADC module (e.g., ADC0_BASE)
    uint32_t gpio_port_base;    // Base address of the GPIO port (e.g., GPIO_PORTE_BASE)
    uint8_t adc_pins[4];        // ADC pins (e.g., GPIO_PIN_3, GPIO_PIN_2, etc.)
    uint8_t adc_channels[4];    // ADC channels corresponding to the pins (e.g., ADC_CTL_CH0, ADC_CTL_CH1, etc.)
    uint8_t num_channels;       // Number of channels to configure
    float sampling_rate;        // Sampling rate in Hz
    uint32_t timer_base;        // Base address of the timer module (e.g., TIMER0_BASE)
    uint32_t timer_peripheral;  // Timer peripheral constant (e.g., SYSCTL_PERIPH_TIMER0)
    uint32_t timer_interrupt;   // Timer interrupt constant (e.g., TIMER_TIMA_TIMEOUT)
    uint32_t timer_instance;
} ADC_Config;


static const ADC_Config adc_config = {
    .gpio_peripheral  = SYSCTL_PERIPH_GPIOE,
    .adc_peripheral = SYSCTL_PERIPH_ADC0,
    .sequencer = 0,
    .adc_base = ADC0_BASE,
    .gpio_port_base = GPIO_PORTE_BASE,
    .adc_pins = {GPIO_PIN_1, GPIO_PIN_2, GPIO_PIN_3, GPIO_PIN_4}, // PE1, PE2, PE3, PE4
    .adc_channels = {ADC_CTL_CH2, ADC_CTL_CH1, ADC_CTL_CH0, ADC_CTL_CH9}, // AIN0, AIN1, AIN2, AIN3
    .num_channels = 4,
    .sampling_rate = 10.0, // 10 Hz
    .timer_base = TIMER0_BASE,
    .timer_peripheral = SYSCTL_PERIPH_TIMER0,
    .timer_interrupt = TIMER_TIMA_TIMEOUT,
    .timer_instance = TIMER_A
};


volatile uint32_t adcValues[8];
volatile float input_voltage = 0.0;
volatile float calculated_resistance[4] = {0, 0, 0, 0};
float sampling_rate = 10;  // sampling rate = 10Hz


//void config_ADC(void){
//    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
//    GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_3); // PE3 as ADC input
//
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
//    TimerLoadSet(TIMER0_BASE, TIMER_A, SysCtlClockGet()/sampling_rate  - 1);
//    TimerIntRegister(TIMER0_BASE, TIMER_A, Timer0AIntHandler);
//    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
//    TimerEnable(TIMER0_BASE, TIMER_A);
//
//    // enable global interrupts
//    IntMasterEnable();
//
//}


void config_ADC(void) {
    // Enable GPIOE and ADC0 peripherals
//    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
//    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
//    GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_4|GPIO_PIN_3|GPIO_PIN_2|GPIO_PIN_1);
//
//    // Configure ADC sequence 1
//    ADCSequenceConfigure(ADC0_BASE, 0, ADC_TRIGGER_PROCESSOR, 0);
//
//    // Configure steps for four sensors
//    ADCSequenceStepConfigure(ADC0_BASE, 0, 0, ADC_CTL_CH2);
//    ADCSequenceStepConfigure(ADC0_BASE, 0, 1, ADC_CTL_CH1);
//    ADCSequenceStepConfigure(ADC0_BASE, 0, 2, ADC_CTL_CH0);
//    ADCSequenceStepConfigure(ADC0_BASE, 0, 3, ADC_CTL_CH9);
//    ADCSequenceStepConfigure(ADC0_BASE, 0, 4, ADC_CTL_CH2);
//    ADCSequenceStepConfigure(ADC0_BASE, 0, 5, ADC_CTL_CH1);
//    ADCSequenceStepConfigure(ADC0_BASE, 0, 6, ADC_CTL_CH0);
//    ADCSequenceStepConfigure(ADC0_BASE, 0, 7, ADC_CTL_CH9 | ADC_CTL_IE | ADC_CTL_END);
//
//    // Enable ADC sequence
//    ADCSequenceEnable(ADC0_BASE, 0);
//
//    ADCHardwareOversampleConfigure(ADC0_BASE,32);


    uint8_t gpio_pins = adc_config.adc_pins[0] | adc_config.adc_pins[1] | adc_config.adc_pins[2] | adc_config.adc_pins[3];

    SysCtlPeripheralEnable(adc_config.gpio_peripheral);
    SysCtlPeripheralEnable(adc_config.adc_peripheral);
    GPIOPinTypeADC(adc_config.gpio_port_base, gpio_pins);
//    GPIOPinTypeADC(adc_config.gpio_port_base, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_4);
//    GPIOPinTypeADC(adc_config.gpio_port_base, adc_config.adc_pins[0] | adc_config.adc_pins[1] | adc_config.adc_pins[2] | adc_config.adc_pins[3]);

    ADCSequenceConfigure(adc_config.adc_base, adc_config.sequencer, ADC_TRIGGER_PROCESSOR, 0);
    ADCSequenceStepConfigure(adc_config.adc_base, adc_config.sequencer, 0, adc_config.adc_channels[0]);
    ADCSequenceStepConfigure(adc_config.adc_base, adc_config.sequencer, 1, adc_config.adc_channels[1]);
    ADCSequenceStepConfigure(adc_config.adc_base, adc_config.sequencer, 2, adc_config.adc_channels[2]);
    ADCSequenceStepConfigure(adc_config.adc_base, adc_config.sequencer, 3, adc_config.adc_channels[3]);
    ADCSequenceStepConfigure(adc_config.adc_base, adc_config.sequencer, 4, adc_config.adc_channels[0]);
    ADCSequenceStepConfigure(adc_config.adc_base, adc_config.sequencer, 5, adc_config.adc_channels[1]);
    ADCSequenceStepConfigure(adc_config.adc_base, adc_config.sequencer, 6, adc_config.adc_channels[2]);
    ADCSequenceStepConfigure(adc_config.adc_base, adc_config.sequencer, 7, adc_config.adc_channels[3] | ADC_CTL_IE | ADC_CTL_END);
    ADCSequenceEnable(adc_config.adc_base, adc_config.sequencer);


    // Configure timer for sampling
    SysCtlPeripheralEnable(adc_config.timer_peripheral);
    TimerConfigure(adc_config.timer_base, TIMER_CFG_PERIODIC);
    TimerLoadSet(adc_config.timer_base, adc_config.timer_instance, SysCtlClockGet() / adc_config.sampling_rate - 1);
    TimerIntRegister(adc_config.timer_base, adc_config.timer_instance, Timer0AIntHandler);
    TimerIntEnable(adc_config.timer_base, adc_config.timer_interrupt);
    TimerEnable(adc_config.timer_base, adc_config.timer_instance);
//    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
//    TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
//    TimerLoadSet(TIMER0_BASE, TIMER_A, SysCtlClockGet()/adc_config.sampling_rate  - 1);
//    TimerIntRegister(TIMER0_BASE, TIMER_A, Timer0AIntHandler);
//    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
//    TimerEnable(TIMER0_BASE, TIMER_A);

    // Enable global interrupts
    IntMasterEnable();
}


//float* calculate_resistence(void)
//{
////    float avg_adc_value = (adcValues[0] + adcValues[1] + adcValues[2] + adcValues[3]) / 4.0;
////    input_voltage = avg_adc_value * (3.3 / 4096.0);
////    float ref_res = 10000.0;
////    calculated_resistance = ref_res * (input_voltage / (3.3));
//    int i;
//    for (i = 0; i < 4; i++) {
//        float avg_adc_value = (adcValues[i] + adcValues[i+4]) / 2.0;
//        input_voltage = avg_adc_value * (3.3 / 4096.0);
//        float ref_res = 10000.0;
//        calculated_resistance[i] = ref_res * (input_voltage / (3.3));
//
//    }
//
//    return calculated_resistance;
//}


void calculate_resistance(void) {
    int i;
    for (i = 0; i < 4; i++) {
        float avg_adc_value = (adcValues[i] + adcValues[i + 4]) / 2.0;
        float input_voltage = avg_adc_value * (3.3 / 4096.0);
        float ref_res = 10000.0;
        calculated_resistance[i] = ref_res * (input_voltage / 3.3);
    }
}

void update_sensors_data(void){
    ADCProcessorTrigger(adc_config.adc_base, adc_config.sequencer);
    while (!ADCIntStatus(adc_config.adc_base, adc_config.sequencer, false));
    ADCIntClear(adc_config.adc_base, adc_config.sequencer);
    ADCSequenceDataGet(adc_config.adc_base, adc_config.sequencer, (uint32_t *)adcValues);

    calculate_resistance();
}

void Timer0AIntHandler(void)
{
//    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
//
//    ADCProcessorTrigger(ADC0_BASE, 0);
//    while (!ADCIntStatus(ADC0_BASE, 0, false));
//    ADCIntClear(ADC0_BASE, 0);
//    ADCSequenceDataGet(ADC0_BASE, 0, (uint32_t *)adcValues);

    TimerIntClear(adc_config.timer_base, adc_config.timer_interrupt);

    update_sensors_data();

//    ADCProcessorTrigger(adc_config.adc_base, adc_config.sequencer);
//    while (!ADCIntStatus(adc_config.adc_base, adc_config.sequencer, false));
//    ADCIntClear(adc_config.adc_base, adc_config.sequencer);
//    ADCSequenceDataGet(adc_config.adc_base, adc_config.sequencer, (uint32_t *)adcValues);

    // calculate and send resistance
//    calculate_resistance();
}




