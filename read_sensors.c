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

#define NUM_CHANNELS 3

typedef struct {
    uint32_t gpio_peripheral ;                  // GPIO peripheral
    uint32_t adc_peripheral;
    int sequencer;
    uint32_t adc_base;                          // Base address of the ADC module (e.g., ADC0_BASE)
    uint32_t gpio_port_base;                    // Base address of the GPIO port (e.g., GPIO_PORTE_BASE)
    uint8_t adc_pins[NUM_CHANNELS];                        // ADC pins (e.g., GPIO_PIN_3, GPIO_PIN_2, etc.)
    uint8_t adc_channels[NUM_CHANNELS];                    // ADC channels corresponding to the pins (e.g., ADC_CTL_CH0, ADC_CTL_CH1, etc.)
    uint8_t num_channels;                       // Number of channels to configure
    float sampling_rate;                        // Sampling rate in Hz
    int oversampling;
} ADC_Config;


const ADC_Config adc_config = {
    .gpio_peripheral  = SYSCTL_PERIPH_GPIOE,
    .adc_peripheral = SYSCTL_PERIPH_ADC0,
    .sequencer = 0,                             // Allow 8 steps for each trigger
    .adc_base = ADC0_BASE,
    .gpio_port_base = GPIO_PORTE_BASE,
//    .adc_pins = {GPIO_PIN_1, GPIO_PIN_2, GPIO_PIN_3, GPIO_PIN_4}, // PE1, PE2, PE3, PE4
//    .adc_channels = {ADC_CTL_CH2, ADC_CTL_CH1, ADC_CTL_CH0, ADC_CTL_CH9}, // AIN2, AIN1, AIN0, AIN9
    .adc_pins = {GPIO_PIN_3, GPIO_PIN_4, GPIO_PIN_5}, // PE3, PE4
    .adc_channels = {ADC_CTL_CH0, ADC_CTL_CH9, ADC_CTL_CH8}, // AIN0, AIN9
    .num_channels = 4,
    .sampling_rate = 40,      // 40 Hz
    .oversampling = 32,
};


volatile uint32_t adcValues[8];
volatile float input_voltage = 0.0;
volatile float calculated_resistance[4] = {0, 0, 0, 0};
volatile float  pannel_voltage = 0;


void config_ADC(void) {
    uint8_t gpio_pins = 0;
    int i;
    for (i = 0; i < NUM_CHANNELS; i++) {
        gpio_pins |= adc_config.adc_pins[i];
    }

    SysCtlPeripheralEnable(adc_config.gpio_peripheral);
    SysCtlPeripheralEnable(adc_config.adc_peripheral);
    GPIOPinTypeADC(adc_config.gpio_port_base, gpio_pins);

    // ADC sequence should adapt to your own sequencer and input pins
    // see  Table13-1 and Table 13-2 (https://www.ti.com/lit/ds/symlink/tm4c123gh6pm.pdf?ts=1695554778406&ref_url=https%253A%252F%252Fwww.google.com%252F)
    ADCSequenceConfigure(adc_config.adc_base, adc_config.sequencer, ADC_TRIGGER_PROCESSOR, 0);  // trigger ADC
    ADCSequenceStepConfigure(adc_config.adc_base, adc_config.sequencer, 0, adc_config.adc_channels[0]);
    ADCSequenceStepConfigure(adc_config.adc_base, adc_config.sequencer, 1, adc_config.adc_channels[1]);
    ADCSequenceStepConfigure(adc_config.adc_base, adc_config.sequencer, 2, adc_config.adc_channels[0]);
    ADCSequenceStepConfigure(adc_config.adc_base, adc_config.sequencer, 3, adc_config.adc_channels[1]);
    ADCSequenceStepConfigure(adc_config.adc_base, adc_config.sequencer, 4, adc_config.adc_channels[2] | ADC_CTL_IE | ADC_CTL_END); // stop sampling
    ADCSequenceEnable(adc_config.adc_base, adc_config.sequencer);
    ADCHardwareOversampleConfigure(adc_config.adc_base, adc_config.oversampling);
}


void update_sensors_data(void){
    ADCIntClear(adc_config.adc_base, adc_config.sequencer);
    ADCProcessorTrigger(adc_config.adc_base, adc_config.sequencer);
    while (!ADCIntStatus(adc_config.adc_base, adc_config.sequencer, false));
    ADCSequenceDataGet(adc_config.adc_base, adc_config.sequencer, (uint32_t *)adcValues);

    calculate_resistance();
    calculate_pannel_voltage();
}


void calculate_resistance(void) {
    int i;
    for (i = 0; i < NUM_CHANNELS-1; i++) {
        float avg_adc_value = (adcValues[i] + adcValues[i + NUM_CHANNELS-1]) / 2.0;
        float input_voltage = avg_adc_value * (3.3 / 4096.0);
        float ref_res = 10000.0;
        calculated_resistance[i] = ref_res * (input_voltage / 3.3);
    }
}

float get_sampling_rate(void){
    return adc_config.sampling_rate;
}

float* get_ldr_data(void){
    return (float*)  calculated_resistance;
}

void calculate_pannel_voltage(void){
    float avg_adc_value = adcValues[4];
    float input_voltage = avg_adc_value * (3.3 / 4096.0);
    pannel_voltage = input_voltage;
}

float get_pannel_voltage(void){
    return pannel_voltage;
}

