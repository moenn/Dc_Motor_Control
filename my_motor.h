/*
 * my_motor.h
 *
 *  Created on: May 29, 2019
 *      Author: jms29
 */

#ifndef MY_MOTOR_H_
#define MY_MOTOR_H_

extern void InitMotor(void);

extern void MotorStart(void);
extern void MotorStop(void);
extern void MotorForward(void);
extern void MotorReverse(void);
 
extern int IsMotorRunning(void);
extern int IsMotorForward(void);
extern int IsMotorReverse(void);

extern void EnableMotorStart(void);
extern void DisableMotorStart(void);
extern void EnablePwmChange(void);
extern void DisablePwmChange(void);

#endif /* MY_MOTOR_H_ */
