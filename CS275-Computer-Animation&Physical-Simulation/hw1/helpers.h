#ifndef HELPERS_H_
#define HELPERS_H_

#include <math.h>

void eularAngleToQuat(float eAngle[3], float quat[4])
{
    float alpha = eAngle[0], beta = eAngle[1], gamma = eAngle[2];
    double cosa = cos(alpha * 0.5);
    double sina = sin(alpha * 0.5);
    double cosb = cos( beta * 0.5);
    double sinb = sin( beta * 0.5);
    double cosg = cos(gamma * 0.5);
    double sing = sin(gamma * 0.5);
    quat[0] = cosg * cosb * cosa + sing * sinb * sina;
    quat[1] = sing * cosb * cosa - cosg * sinb * sina;
    quat[2] = cosg * sinb * cosa + sing * cosb * sina;
    quat[3] = cosg * cosb * sina - sing * sinb * cosa;
}

void quatToEularAngle(float quat[4], float eAngle[3])
{
    float q0 = quat[0], q1 = quat[1], q2 = quat[2], q3 = quat[3];
    // rotate up-down, initial = pi
    eAngle[0] = atan2(2 * (q0 * q3 + q1 * q2), 1 - 2 * (q2 * q2 + q3 * q3)); 
    // rotate left-right, initial = 0
    eAngle[1] = asin(2 * (q0 * q2 - q1 * q3)); 
    // rotate rolling, initial = 0
    eAngle[2] = atan2(2 * (q0 * q1 + q2 * q3), 1 - 2 * (q1 * q1 + q2 * q2)); 
}

#endif