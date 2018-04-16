# CarND-Controls-PID
Self-Driving Car Engineer Nanodegree Program

---

## Dependencies

* cmake >= 3.5
 * All OSes: [click here for installation instructions](https://cmake.org/install/)
* make >= 4.1(mac, linux), 3.81(Windows)
  * Linux: make is installed by default on most Linux distros
  * Mac: [install Xcode command line tools to get make](https://developer.apple.com/xcode/features/)
  * Windows: [Click here for installation instructions](http://gnuwin32.sourceforge.net/packages/make.htm)
* gcc/g++ >= 5.4
  * Linux: gcc / g++ is installed by default on most Linux distros
  * Mac: same deal as make - [install Xcode command line tools]((https://developer.apple.com/xcode/features/)
  * Windows: recommend using [MinGW](http://www.mingw.org/)
* [uWebSockets](https://github.com/uWebSockets/uWebSockets)
  * Run either `./install-mac.sh` or `./install-ubuntu.sh`.
  * If you install from source, checkout to commit `e94b6e1`, i.e.
    ```
    git clone https://github.com/uWebSockets/uWebSockets 
    cd uWebSockets
    git checkout e94b6e1
    ```
    Some function signatures have changed in v0.14.x. See [this PR](https://github.com/udacity/CarND-MPC-Project/pull/3) for more details.
* Simulator. You can download these from the [project intro page](https://github.com/udacity/self-driving-car-sim/releases) in the classroom.

There's an experimental patch for windows in this [PR](https://github.com/udacity/CarND-PID-Control-Project/pull/3)

## Basic Build Instructions

1. Clone this repo.
2. Make a build directory: `mkdir build && cd build`
3. Compile: `cmake .. && make`
4. Run it: `./pid`. 

Tips for setting up your environment can be found [here](https://classroom.udacity.com/nanodegrees/nd013/parts/40f38239-66b6-46ec-ae68-03afd8a601c8/modules/0949fca6-b379-42af-a919-ee50aa304e6a/lessons/f758c44c-5e40-4e01-93b5-1a82aa4e044f/concepts/23d376c7-0195-4276-bdf0-e02f1f3c665d)

## PID Components
### The PID algorithm is composed of three components: P, I, and D.
### The "P" stands for proportional, which is used to steer the vehicle back to the center of the road in some proportion of the cross track error (CTE). For instance, in my implementation I set the "P" coefficient (Kp) to 0.06. This means that if the CTE is -5, the steering angle is adjusted to .3. This results in a gradual correction towards the center of the lane based on the CTE. The "I" stands for integral, and keeps a running sum of the CTE. This is particularly helpful for cars exhibiting some sort of steering bias, such as pulling to the right. In my implementation I found it helpful for going around turns, as the effects of the other components (P & D) were not compensating enough to stay in the middle of the lane during a sharp curve. Setting the "I" coefficient (Ki) to a very small value (0.002) was enough to improve performance. Lastly, the "D" component stands for derivative and its purpose is to prevent overcorrection from the "P" component. When the car is steering towards the center of the lane to adjust for the CTE, the derivative will become negative, so the "D" coefficient (Kd) will reduce the absolute value of the steering angle produced by the "P" component. The result is a smooth adjustment toward the center of the lane, instead of constant overcorrection resulting in a "wobbly" vehicle.

## Parameter Tuning
### I found the PID coefficients through manual trial and error. I started by only setting Kp to 0.1. This led to a car that steered >toward the center of the lane, but would drastically overshoot. I continued to reduce the Kp to 0.05 before introducing the "D" component to reduce overcorrection. I initially set Kd to 1.0, which turned out to vastly improve performance. Just using these values of Kp = 0.1 and Kd = 1.0 was enough to get the car around the track reasonably well, though the vehicle would go up on the curb during sharp turns. After a lot more fiddling with the Kp and Kd coefficients, I decided to test the effects of the "I" component. I found that setting Ki to a small value of 0.002 solved my problem of the car going up on the curb during turns. After more fine tuning, I settled on coefficient values of Kp = 0.06 Ki = 0.002 and Kd = 1.1 which results in a smooth ride around the track.
