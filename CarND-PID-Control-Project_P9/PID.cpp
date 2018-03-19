#include "PID.h"

using namespace std;

/*
* TODO: Complete the PID class.
*/

PID::PID() {}

PID::~PID() {}

void PID::Init(double Kp, double Ki, double Kd) {
    Kp_=Kp;
    Ki_=Ki;
    Kd_=Kd;
    d_error = 0;
    p_error = 0;
    i_error = 0;
}

void PID::UpdateError(double cte) {
    d_error = cte - p_error;
    p_error = cte;
    i_error += cte;
}

double PID::Signal() {
    return - d_error * Kd_ - p_error * Kp_ - i_error * Ki_;
}

double PID::TotalError() {
}
