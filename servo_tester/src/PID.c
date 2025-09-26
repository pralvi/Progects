#include "PID.h"

//http://roboforum.ru/wiki/Перевод_статьи_"Просто_о_ПИД-алгоритмах"
PID InitPID(double Kp, double Ki, double Kd, double MaxEffort) {
    return (PID) {Kp, Ki, Kd, -MaxEffort/Ki, MaxEffort/Ki, 0, 0};
}

void InitKi(PID* pid, double Ki, double MaxEffort) {
    pid->Ki = Ki;
    pid->iMin = -(MaxEffort*0.15)/Ki;
    pid->iMax = (MaxEffort*0.15)/Ki;
}

double CalculatePID(PID * pid, double error, double position) {
    double pTerm, dTerm, iTerm;

    pTerm = pid->Kp * error;    // calculate the proportional term

    if (error == 0)
        pid->iState = 0;
    else {
        pid->iState += error;       // calculate the integral state with appropriate limiting
        if (pid->iState > pid->iMax)
            pid->iState = pid->iMax;
        else if (pid->iState < pid->iMin)
            pid->iState = pid->iMin;
    }
    iTerm = pid->Ki * pid->iState;// calculate the integral term

    dTerm = pid->Kd * (position - pid->dState);      // calculate the differential term
    pid->dState = position;

    return (pTerm + iTerm - dTerm);
}
