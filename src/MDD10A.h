#ifndef MDD10A_h
#define MDD10A_h

#include <Arduino.h>

class MDD10A {

public:

	MDD10A();

	void setMotorSpeed(int controller, int speed);
	void init();

	int dir = 0;


	//PinOuts

	//MC
	const int _mc0_pwm0 = 2; //LEFT 1
	const int _mc0_dir0 = 28;

	const int _mc0_pwm1 = 10; //RIGTH 1
	const int _mc0_dir1 = 42;

	//MC3
	const int _mc1_pwm0 = 3; //LEFT 2
	const int _mc1_dir0 = 26;

	const int _mc1_pwm1 = 11; //RIGHT 2
	const int _mc1_dir1 = 43;

	//MC
	const int _mc2_pwm0 = 9; //LIFT?
	const int _mc2_dir0 = 30;

	const int _mc2_pwm1 = 8; //LIFT?
	const int _mc2_dir1 = 32;








};

#endif


