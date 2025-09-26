#ifdef __cplusplus
extern "C" {
#endif
//------------------------------------------------------------------------------
#ifndef _PID_H_
#define _PID_H_
//-----------------------------------------------------------------------------

typedef struct
{
  double Kp,            // proportional gain
         Ki,            // integral gain
         Kd;            // derivative gain
  double iMin, iMax;    // Minimum and maximum allowable integrator state
  double dState;        // Last position input
  double iState;        // Integrator state
} PID;

PID InitPID(double Kp, double Ki, double Kd, double MaxEffort);
void InitKi(PID* pid, double Ki, double MaxEffort);
double CalculatePID(PID * pid, double error, double position);

//-----------------------------------------------------------------------------
#endif /* _PID_H_ */
//-----------------------------------------------------------------------------
#ifdef __cplusplus
}
#endif /* _cplusplus*/
