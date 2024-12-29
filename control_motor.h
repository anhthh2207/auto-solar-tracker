#ifndef CONTROL_MOTOR_H
#define CONTROL_MOTOR_H

void init_PWM(void);
float calculate_pulse_percent(int angle);
void set_motor0_angle(int angle);
void set_motor1_angle(int angle);

#endif
