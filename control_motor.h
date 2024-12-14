#ifndef CONTROL_MOTOR_H
#define CONTROL_MOTOR_H

//extern void configure_PWM(void);
//extern float calculate_pulse_percent(int angle);
//extern void set_motor_angle(int angle);
void configure_PWM(void);
float calculate_pulse_percent(int angle);
void set_motor_angle(int angle);

#endif
