#ifndef CONTROL_MOTOR_H
#define CONTROL_MOTOR_H

void init_PWM(void);
float calculate_pulse_percent(int angle);
void set_motor_angle(int angle);

#endif
