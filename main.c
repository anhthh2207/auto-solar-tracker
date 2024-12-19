// using sequencer 0 for all 4 pin + 1 generator for PWM + UART for debug
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

void SysTickIntHandler(void);

uint32_t ui32ADC0ValueSeq0[8];


volatile float ui32DigitalValueLeft;
volatile float ui32VoltageLeft;

volatile float ui32DigitalValueRight;
volatile float ui32VoltageRight;

volatile float ui32DigitalValueTop;
volatile float ui32VoltageTop;

volatile float ui32DigitalValueBottom;
volatile float ui32VoltageBottom;

volatile uint32_t ui32PWMClock;
volatile uint32_t ui32Load;
volatile uint32_t ui8AdjustLeftRight;
volatile uint32_t ui8AdjustTopBottom;

char acBuffer[100];

int posLeftRight = 90;
int posTopBottom = 90;
int tolerance = 75;

int main(void)
{

    ui8AdjustLeftRight = 75; // match 1.5ms value with pwm frequency of 50Hz
    ui8AdjustTopBottom = 75;

    SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);

    //ADC Input Pin
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_4|GPIO_PIN_3|GPIO_PIN_2|GPIO_PIN_1);

    SysCtlPWMClockSet(SYSCTL_PWMDIV_64); // set divider = 64 -> PWM clock: 625kHz
    SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM1); //Set PWM module

    //PWM signal PF1 and PF2 (Module 1, Generator 2, Generator 3)
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    GPIOPinTypePWM(GPIO_PORTF_BASE,GPIO_PIN_2|GPIO_PIN_3);
    GPIOPinConfigure(GPIO_PF3_M1PWM7);
    GPIOPinConfigure(GPIO_PF2_M1PWM6);

    //UART Signal
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    // Config the UART interrupt
    UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200, (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));


    // Setup ADC sequence ADC module 0
    ADCHardwareOversampleConfigure(ADC0_BASE,32);

    ADCSequenceConfigure(ADC0_BASE, 0, ADC_TRIGGER_PROCESSOR, 0); //pin PE2 - Left
    ADCSequenceStepConfigure(ADC0_BASE, 0, 0, ADC_CTL_CH9); // Pin PE4 - Top
    ADCSequenceStepConfigure(ADC0_BASE, 0, 1, ADC_CTL_CH9);
    ADCSequenceStepConfigure(ADC0_BASE, 0, 2, ADC_CTL_CH0); // Pin PE3 - Bottom
    ADCSequenceStepConfigure(ADC0_BASE, 0, 3, ADC_CTL_CH0);

    //pin PE1, sequencer 2 - Right
    ADCSequenceStepConfigure(ADC0_BASE, 0, 4, ADC_CTL_CH1); // Pin PE2 - Left
    ADCSequenceStepConfigure(ADC0_BASE, 0, 5, ADC_CTL_CH1);
    ADCSequenceStepConfigure(ADC0_BASE, 0, 6, ADC_CTL_CH2); // Pin PE1 - Right
    ADCSequenceStepConfigure(ADC0_BASE, 0, 7, ADC_CTL_CH2|ADC_CTL_IE|ADC_CTL_END);
    ADCSequenceEnable(ADC0_BASE,0);


    // Setup PWM pin PF2 - Top Bottom
    PWMGenConfigure(PWM1_BASE, PWM_GEN_3, PWM_GEN_MODE_DOWN);

    #define PWM_FREQUENCY 50 // every second, the system finish counting 200 ->freq 200
    ui32PWMClock = SysCtlClockGet() / 64;
    ui32Load = (ui32PWMClock / PWM_FREQUENCY) -1; // count from 3125 to 0 (load 3125 to load); sysclk count 625,000 tick/second

    PWMGenPeriodSet(PWM1_BASE,PWM_GEN_3, ui32Load);
    PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6, ui8AdjustTopBottom * ui32Load / 1000); //625 // put 80 here -> 80%
    PWMOutputState(PWM1_BASE,PWM_OUT_6_BIT,true);

    // Setup PWM pin PF3 - Left Right
    PWMPulseWidthSet(PWM1_BASE, PWM_OUT_7, ui8AdjustLeftRight * ui32Load / 1000); //625 // put 80 here -> 80%
    PWMOutputState(PWM1_BASE,PWM_OUT_7_BIT,true);
    PWMGenEnable(PWM1_BASE, PWM_GEN_3);

    // Setup the SysTick
    SysTickPeriodSet(SysCtlClockGet()/20 - 1); // frequecy of systick = 20Hz -> update the ADC/pos 20time/sec
    SysTickIntEnable();

    IntMasterEnable();

    SysTickEnable();


    while(1)
    {


    }


}

void SysTickIntHandler(void)
{
    // ADC input from top bottom - PE4, PE3
    ADCIntClear(ADC0_BASE,0);
    ADCProcessorTrigger(ADC0_BASE,0);
    while(!ADCIntStatus(ADC0_BASE, 0, false))
    {

    }
    ADCSequenceDataGet(ADC0_BASE,0, ui32ADC0ValueSeq0)
    ;
    ui32DigitalValueTop = (ui32ADC0ValueSeq0[0]+ ui32ADC0ValueSeq0[1])/2;
    ui32VoltageTop = ui32DigitalValueTop*1000*(3.3/4096);


    ui32DigitalValueBottom = (ui32ADC0ValueSeq0[2]+ ui32ADC0ValueSeq0[3])/2;
    ui32VoltageBottom = ui32DigitalValueBottom*1000*(3.3/4096);


    ui32DigitalValueLeft = (ui32ADC0ValueSeq0[4]+ ui32ADC0ValueSeq0[5])/2;
    ui32VoltageLeft = ui32DigitalValueLeft*1000*(3.3/4096);


    ui32DigitalValueRight = (ui32ADC0ValueSeq0[6]+ ui32ADC0ValueSeq0[7])/2;
    ui32VoltageRight = ui32DigitalValueRight*1000*(3.3/4096);

    volatile float ui32VoltagedifTopBottom = ui32VoltageBottom - ui32VoltageTop;

    if (fabs(ui32VoltageBottom - ui32VoltageTop) < tolerance)
    {

    }else {
        if(ui32VoltageBottom > ui32VoltageTop){
            posTopBottom = posTopBottom + 1;
        }
        if(ui32VoltageBottom < ui32VoltageTop){
            posTopBottom = posTopBottom - 1;
        }
    }

    if(posTopBottom > 180) {posTopBottom = 180;}
    if(posTopBottom < 0) {posTopBottom = 0;}
    ui8AdjustTopBottom = (int) (25 + posTopBottom*100/180);
    PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6, ui8AdjustTopBottom * ui32Load / 1000); //ui8Adjust -> integer -> 5 muc -> float trc -> int

    volatile float ui32VoltagedifLeftRight = ui32VoltageRight - ui32VoltageLeft;

    if (fabs(ui32VoltageRight - ui32VoltageLeft) < tolerance)
    {

    }else {
        if(ui32VoltageRight > ui32VoltageLeft){
            posLeftRight = posLeftRight + 1;
        }
        if(ui32VoltageRight < ui32VoltageLeft){
            posLeftRight = posLeftRight - 1;
        }
    }

    if(posLeftRight > 180) {posLeftRight = 180;}
    if(posLeftRight < 0) {posLeftRight = 0;}

    ui8AdjustLeftRight = (int) (25 + posLeftRight*100/180);
    PWMPulseWidthSet(PWM1_BASE, PWM_OUT_7, ui8AdjustLeftRight * ui32Load / 1000); //ui8Adjust -> integer -> 5 muc -> float trc -> int


    uint32_t noBytes = sprintf(acBuffer, "vdifTB: %.1f\t posTB: %d\t duty cycleTB: %d\t vdifLR: %.1f\t posLR: %d\t duty cycleLR: %d\n", ui32VoltagedifTopBottom, posTopBottom, ui8AdjustTopBottom, ui32VoltagedifLeftRight, posLeftRight, ui8AdjustLeftRight);
    uint32_t index = 0;

    for(index = 0; index < noBytes; index++){
        UARTCharPut(UART0_BASE, acBuffer[index]);
    }
}



