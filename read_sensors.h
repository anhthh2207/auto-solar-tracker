#ifndef READ_SENSORS_H
#define READ_SENSORS_H

void config_ADC(void);
void calculate_resistance(void);
void update_sensors_data(void);
float get_sampling_rate(void);
float* get_ldr_data(void);
void calculate_pannel_voltage(void);
float get_pannel_voltage(void);

#endif
