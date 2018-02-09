#include "XBOXRECV.h"
#include "MDD10A.h"
#include "SimpleTimer.h"

USB Usb;
XBOXRECV Xbox(&Usb);
MDD10A mc;
SimpleTimer t;

float LeftJoystickY;
float LeftJoystickX;
float RightJoystickY;
float RightJoystickX;
float TriggerAggregate;

float DriveRightSpeed;
float DriveLeftSpeed;
float MOGOSpeed;

bool SwitchOverride;

float isArcadeDrive;

void timerLoop();
void ReadController();
void Task();
void WriteRobot();
void Drive(int l, int r);
void MOGO(int s);

void setup()
{
	pinMode(34, INPUT_PULLUP);

	LeftJoystickY = 0.0;
	LeftJoystickX = 0.0;
	RightJoystickY = 0.0;
	RightJoystickX = 0.0;
	TriggerAggregate = 0.0;

	DriveRightSpeed = 0.0;
	DriveLeftSpeed = 0.0;
	MOGOSpeed = 0.0;
	SwitchOverride = false;
	isArcadeDrive = false;

	Serial.begin(115200);

	if (Usb.Init() == -1)
	{
		Serial.print(F("\r\nOSC did not start"));
		while (1)
			; //halt
	}
	Serial.print(F("\r\nXbox Wireless Receiver Library Started"));

	t.setInterval(20, timerLoop);
}

void loop()
{
	Usb.Task();
	t.run();
}

void timerLoop()
{
	ReadController();
	Task();
	WriteRobot();
}

void WriteRobot()
{
	mc.setMotorSpeed(0, DriveRightSpeed);
	mc.setMotorSpeed(2, DriveRightSpeed);

	mc.setMotorSpeed(1, DriveLeftSpeed);
	mc.setMotorSpeed(3, DriveLeftSpeed);

	mc.setMotorSpeed(4, MOGOSpeed);
	mc.setMotorSpeed(5, MOGOSpeed);

	//Serial.println(DriveRightSpeed);
}

void Task()
{

	if (!isArcadeDrive)
	{
		DriveRightSpeed = RightJoystickY;
		DriveLeftSpeed = LeftJoystickY;
	}
	else
	{
		DriveLeftSpeed = (RightJoystickY + LeftJoystickY) + (RightJoystickX + LeftJoystickX);
		DriveRightSpeed = (RightJoystickY + LeftJoystickY) - (RightJoystickX + LeftJoystickX);

		if (DriveLeftSpeed > 255)
			DriveLeftSpeed = 255;

		if (DriveLeftSpeed < -255)
			DriveLeftSpeed = -255;

		if (DriveRightSpeed > 255)
			DriveRightSpeed = 255;

		if (DriveRightSpeed < -255)
			DriveRightSpeed = -255;
	}

	if (SwitchOverride == true)
	{
		MOGOSpeed = TriggerAggregate;
	}
	else
	{
		if (TriggerAggregate > 0 && digitalRead(34) == 0)
			MOGOSpeed = 0;
		else
			MOGOSpeed = TriggerAggregate;
	}

	//Serial.println(digitalRead(34));

	if (false)
	{
		//do exponential

		DriveRightSpeed = map((DriveRightSpeed * abs(DriveRightSpeed)), -65025, 65025, -255, 255);
		DriveLeftSpeed = map((DriveLeftSpeed * abs(DriveLeftSpeed)), -65025, 65025, -255, 255);
	}

	if (false)
	{
		int maxDiff = 60;
		if (DriveLeftSpeed > 0 && DriveRightSpeed > 0)
		{
			if (abs(DriveLeftSpeed - DriveRightSpeed) > maxDiff)
			{
				if (DriveLeftSpeed > DriveRightSpeed)
				{
					DriveRightSpeed = DriveLeftSpeed - maxDiff;
				}
				else
				{
					DriveLeftSpeed = DriveRightSpeed - maxDiff;
				}
			}
		}
		else if (DriveLeftSpeed < 0 && DriveRightSpeed < 0)
		{
			if (abs(DriveLeftSpeed - DriveRightSpeed) > maxDiff)
			{
				if (DriveLeftSpeed < DriveRightSpeed)
				{
					DriveRightSpeed = DriveLeftSpeed - maxDiff;
				}
				else
				{
					DriveLeftSpeed = DriveRightSpeed - maxDiff;
				}
			}
		}
	}
}

void ReadController()
{
	LeftJoystickY = 0.0;
	LeftJoystickX = 0.0;
	RightJoystickY = 0.0;
	RightJoystickX = 0.0;
	TriggerAggregate = 0.0;

	if (Xbox.XboxReceiverConnected)
	{
		for (uint8_t i = 0; i < 4; i++)
		{
			if (Xbox.Xbox360Connected[i])
			{
				//L2 Trigger
				if (Xbox.getButtonPress(R2, i))
				{
					TriggerAggregate = 255.0 / 255 * Xbox.getButtonPress(R2, i) * -1;
				}
				//R2 Trigger
				else if (Xbox.getButtonPress(L2, i))
				{
					TriggerAggregate = 255.0 / 255 * Xbox.getButtonPress(L2, i) * 1;
				}

				if (Xbox.getAnalogHat(LeftHatX, i) > 7500 || Xbox.getAnalogHat(LeftHatX, i) < -7500 || Xbox.getAnalogHat(LeftHatY, i) > 7500 || Xbox.getAnalogHat(LeftHatY, i) < -7500 || Xbox.getAnalogHat(RightHatX, i) > 7500 || Xbox.getAnalogHat(RightHatX, i) < -7500 || Xbox.getAnalogHat(RightHatY, i) > 7500 || Xbox.getAnalogHat(RightHatY, i) < -7500)
				{
					if (Xbox.getAnalogHat(LeftHatX, i) > 7500 || Xbox.getAnalogHat(LeftHatX, i) < -7500)
					{
						LeftJoystickX = 255.0 / 32767 * Xbox.getAnalogHat(LeftHatX, i);
					}
					if (Xbox.getAnalogHat(LeftHatY, i) > 7500)
					{
						//LeftJoystickY = 255.0 / 32767 * Xbox.getAnalogHat(LeftHatY, i);
						LeftJoystickY = map(Xbox.getAnalogHat(LeftHatY, i), 5500, 32767, 116, 255);
					}
					else if (Xbox.getAnalogHat(LeftHatY, i) < -7500)
					{
						LeftJoystickY = map(Xbox.getAnalogHat(LeftHatY, i), -5500, -32767, -116, -255);
					}

					if (Xbox.getAnalogHat(RightHatX, i) > 7500 || Xbox.getAnalogHat(RightHatX, i) < -7500)
					{
						RightJoystickX = 255.0 / 32767 * Xbox.getAnalogHat(RightHatX, i);
					}
					if (Xbox.getAnalogHat(RightHatY, i) > 7500)
					{
						//RightJoystickY = 255.0 / 32767 * Xbox.getAnalogHat(RightHatY, i);
						RightJoystickY = map(Xbox.getAnalogHat(RightHatY, i), 5500, 32767, 116, 255);
					}
					else if (Xbox.getAnalogHat(RightHatY, i) < -7500)
					{
						RightJoystickY = map(Xbox.getAnalogHat(RightHatY, i), -5500, -32767, -116, -255);
					}
				}
				Serial.println(Xbox.getAnalogHat(LeftHatY, i));

				if (Xbox.getButtonPress(UP, i))
				{
					LeftJoystickY = 200;
					RightJoystickY = 200;
				}

				if (Xbox.getButtonClick(START, i))
				{
					isArcadeDrive = !isArcadeDrive;
				}
				if (Xbox.getButtonClick(B, i))
				{
					SwitchOverride = !SwitchOverride;
				}



				if (Xbox.getButtonClick(XBOX, i))
				{
					Drive(150,150);
					delay(1000);

					Drive(0,0);
				}

				if (Xbox.getButtonClick(UP, i))
				{
					
					MOGO(-200);
					delay(650);

					MOGO(0);
					Drive(250,250);
					delay(300);

					Drive(0,0);
					delay(500);


					Drive(250,250);
					delay(700);

					MOGO(200);
					delay(1000);

					MOGO(0);
					delay(1300);
				    
                    Drive(0,0);
                    delay(500);

					MOGO(-200);
					delay(300);
					MOGO(0);

					Drive(-255,-255);
					delay(1500);

					Drive(0,0);

				}

					//match auton
				if (Xbox.getButtonClick(DOWN, i))
				{
					
					MOGO(-200);
					delay(650);

					MOGO(0);
					Drive(250,250);
					delay(250);

					Drive(0,0);
					delay(500);


					Drive(150,150);
					delay(450);
					Drive(100,100);
					delay(750);

                    Drive(0,0);
					MOGO(200);
					delay(1000);

					MOGO(0);
					
                    Drive(0,-150);
					delay(1400);

					Drive(150,150);
					delay(1500);
					
					Drive(0,0);
					MOGO(-200);
					delay(300);
					Drive(-250,-250);
					delay(500);
				}
			}
		}
	}
}

void Drive(int l, int r)
{
	mc.setMotorSpeed(1, l);
	mc.setMotorSpeed(3, l);
	mc.setMotorSpeed(0, r);
	mc.setMotorSpeed(2, r);
}

void MOGO(int speed)
{
	mc.setMotorSpeed(4, speed * -1);
	mc.setMotorSpeed(5, speed * -1);
}