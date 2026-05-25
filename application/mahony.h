#ifndef _MAHONY_FILTER_H
#define _MAHONY_FILTER_H

#include "deewaz_math.h"

struct MAHONY_FILTER_t;

struct MAHONY_FILTER_t
{
    float Kp, Ki;
    float dt;
    Axis3f gyro, acc;

    float exInt, eyInt, ezInt;
    float q0, q1, q2, q3;
    float rMat[3][3];

    float pitch, roll, yaw;

    void (*mahony_init)(struct MAHONY_FILTER_t *mf, float Kp, float Ki, float dt);
    void (*mahony_input)(struct MAHONY_FILTER_t *mf, Axis3f gyro, Axis3f acc);
    void (*mahony_update)(struct MAHONY_FILTER_t *mf);
    void (*mahony_output)(struct MAHONY_FILTER_t *mf);
    void (*RotationMatrix_update)(struct MAHONY_FILTER_t *mf);
};

void mahony_init(struct MAHONY_FILTER_t *mf, float Kp, float Ki, float dt);
void mahony_input(struct MAHONY_FILTER_t *mf, Axis3f gyro, Axis3f acc);
void mahony_update(struct MAHONY_FILTER_t *mf);
void mahony_output(struct MAHONY_FILTER_t *mf);
void RotationMatrix_update(struct MAHONY_FILTER_t *mf);

#endif
