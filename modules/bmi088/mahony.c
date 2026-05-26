#include "mahony.h"

void RotationMatrix_update(struct MAHONY_FILTER_t *mf)
{
    float q1q1 = mf->q1 * mf->q1;
    float q2q2 = mf->q2 * mf->q2;
    float q3q3 = mf->q3 * mf->q3;

    float q0q1 = mf->q0 * mf->q1;
    float q0q2 = mf->q0 * mf->q2;
    float q0q3 = mf->q0 * mf->q3;
    float q1q2 = mf->q1 * mf->q2;
    float q1q3 = mf->q1 * mf->q3;
    float q2q3 = mf->q2 * mf->q3;

    mf->rMat[0][0] = 1.0f - 2.0f * q2q2 - 2.0f * q3q3;
    mf->rMat[0][1] = 2.0f * (q1q2 + -q0q3);
    mf->rMat[0][2] = 2.0f * (q1q3 - -q0q2);

    mf->rMat[1][0] = 2.0f * (q1q2 - -q0q3);
    mf->rMat[1][1] = 1.0f - 2.0f * q1q1 - 2.0f * q3q3;
    mf->rMat[1][2] = 2.0f * (q2q3 + -q0q1);

    mf->rMat[2][0] = 2.0f * (q1q3 + -q0q2);
    mf->rMat[2][1] = 2.0f * (q2q3 - -q0q1);
    mf->rMat[2][2] = 1.0f - 2.0f * q1q1 - 2.0f * q2q2;
}

void mahony_input(struct MAHONY_FILTER_t *mf, Axis3f gyro, Axis3f acc)
{
    mf->gyro = gyro;
    mf->acc  = acc;
}

void mahony_update(struct MAHONY_FILTER_t *mf)
{
    float normalise;
    float ex, ey, ez;

    // BMI088 陀螺仪输出已是 rad/s，无需 DEG2RAD 转换

    // 单位化加速度
    normalise = invSqrt(mf->acc.x * mf->acc.x + mf->acc.y * mf->acc.y + mf->acc.z * mf->acc.z);
    mf->acc.x *= normalise;
    mf->acc.y *= normalise;
    mf->acc.z *= normalise;

    // 加速度与重力方向的叉积误差
    ex = (mf->acc.y * mf->rMat[2][2] - mf->acc.z * mf->rMat[2][1]);
    ey = (mf->acc.z * mf->rMat[2][0] - mf->acc.x * mf->rMat[2][2]);
    ez = (mf->acc.x * mf->rMat[2][1] - mf->acc.y * mf->rMat[2][0]);

    // 积分误差累计
    mf->exInt += mf->Ki * ex * mf->dt;
    mf->eyInt += mf->Ki * ey * mf->dt;
    mf->ezInt += mf->Ki * ez * mf->dt;

    // PI 修正陀螺零偏
    mf->gyro.x += mf->Kp * ex + mf->exInt;
    mf->gyro.y += mf->Kp * ey + mf->eyInt;
    mf->gyro.z += mf->Kp * ez + mf->ezInt;

    // 一阶近似，四元数更新
    float q0Last = mf->q0;
    float q1Last = mf->q1;
    float q2Last = mf->q2;
    float q3Last = mf->q3;
    float halfT  = mf->dt * 0.5f;
    mf->q0 += (-q1Last * mf->gyro.x - q2Last * mf->gyro.y - q3Last * mf->gyro.z) * halfT;
    mf->q1 += ( q0Last * mf->gyro.x + q2Last * mf->gyro.z - q3Last * mf->gyro.y) * halfT;
    mf->q2 += ( q0Last * mf->gyro.y - q1Last * mf->gyro.z + q3Last * mf->gyro.x) * halfT;
    mf->q3 += ( q0Last * mf->gyro.z + q1Last * mf->gyro.y - q2Last * mf->gyro.x) * halfT;

    // 单位化四元数
    normalise = invSqrt(mf->q0 * mf->q0 + mf->q1 * mf->q1 + mf->q2 * mf->q2 + mf->q3 * mf->q3);
    mf->q0 *= normalise;
    mf->q1 *= normalise;
    mf->q2 *= normalise;
    mf->q3 *= normalise;

    mf->RotationMatrix_update(mf);
}

void mahony_output(struct MAHONY_FILTER_t *mf)
{
    mf->pitch = -asinf(mf->rMat[2][0]) * RAD2DEG;
    mf->roll  = atan2f(mf->rMat[2][1], mf->rMat[2][2]) * RAD2DEG;
    mf->yaw   = atan2f(mf->rMat[1][0], mf->rMat[0][0]) * RAD2DEG;
}

void mahony_init(struct MAHONY_FILTER_t *mf, float Kp, float Ki, float dt)
{
    mf->Kp = Kp;
    mf->Ki = Ki;
    mf->dt = dt;
    mf->q0 = 1.0f;
    mf->q1 = 0.0f;
    mf->q2 = 0.0f;
    mf->q3 = 0.0f;
    mf->exInt = 0.0f;
    mf->eyInt = 0.0f;
    mf->ezInt = 0.0f;

    mf->rMat[0][0] = 1.0f; mf->rMat[0][1] = 0.0f; mf->rMat[0][2] = 0.0f;
    mf->rMat[1][0] = 0.0f; mf->rMat[1][1] = 1.0f; mf->rMat[1][2] = 0.0f;
    mf->rMat[2][0] = 0.0f; mf->rMat[2][1] = 0.0f; mf->rMat[2][2] = 1.0f;

    mf->mahony_input          = mahony_input;
    mf->mahony_update         = mahony_update;
    mf->mahony_output         = mahony_output;
    mf->RotationMatrix_update = RotationMatrix_update;
}
