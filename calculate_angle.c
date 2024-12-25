#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "calculate_angle.h"

#define TOL 10          // tolerance in %
#define STEP_SIZE 2

int update_new_angle(int angle, float ldr1, float ldr2) {
    if (ldr1 - ldr2 > TOL * ldr2 / 100) {
        angle += STEP_SIZE;
    } else if (ldr2 - ldr1 > TOL * ldr1 / 100) {
        angle -= STEP_SIZE;
    }

    // clamp the angle
    if (angle < 0) {
        angle = 0;
    } else if (angle > 180) {
        angle = 180;
    }

    return angle;
}
