#ifndef READ_SENSORS_H
#define READ_SENSORS_H

void config_ADC(void);
void calculate_resistance(void);
void Timer0AIntHandler(void);
void update_sensors_data(void);

#endif
