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

void setup() {
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

	if (Usb.Init() == -1) {
		Serial.print(F("\r\nOSC did not start"));
		while (1); //halt
	}
	Serial.print(F("\r\nXbox Wireless Receiver Library Started"));

	t.setInterval(20, timerLoop);
}

void loop() {
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

void Task() {
	
	if (!isArcadeDrive)
	{
		DriveRightSpeed = RightJoystickY;
		DriveLeftSpeed = LeftJoystickY;
	}
	else
	{
		DriveLeftSpeed = (RightJoystickY + LeftJoystickY) + (RightJoystickX+LeftJoystickX);
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

	if (true)
	{
		//do exponential

		DriveRightSpeed = map((DriveRightSpeed * abs(DriveRightSpeed)), -65025, 65025, -255, 255);
		DriveLeftSpeed = map((DriveLeftSpeed * abs(DriveLeftSpeed)), -65025, 65025, -255, 255);
	}

	if (true)
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

	if (Xbox.XboxReceiverConnected) {
		for (uint8_t i = 0; i < 4; i++) {
			if (Xbox.Xbox360Connected[i]) {
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

				if (Xbox.getAnalogHat(LeftHatX, i) > 7500 || Xbox.getAnalogHat(LeftHatX, i) < -7500 || Xbox.getAnalogHat(LeftHatY, i) > 7500 || Xbox.getAnalogHat(LeftHatY, i) < -7500 || Xbox.getAnalogHat(RightHatX, i) > 7500 || Xbox.getAnalogHat(RightHatX, i) < -7500 || Xbox.getAnalogHat(RightHatY, i) > 7500 || Xbox.getAnalogHat(RightHatY, i) < -7500) {
					if (Xbox.getAnalogHat(LeftHatX, i) > 7500 || Xbox.getAnalogHat(LeftHatX, i) < -7500) {
						LeftJoystickX = 255.0 / 32767 * Xbox.getAnalogHat(LeftHatX, i);
					}
					if (Xbox.getAnalogHat(LeftHatY, i) > 7500 || Xbox.getAnalogHat(LeftHatY, i) < -7500) {
						LeftJoystickY = 255.0 / 32767 * Xbox.getAnalogHat(LeftHatY, i);
					}
					if (Xbox.getAnalogHat(RightHatX, i) > 7500 || Xbox.getAnalogHat(RightHatX, i) < -7500) {
						RightJoystickX = 255.0 / 32767 * Xbox.getAnalogHat(RightHatX, i);
					}
					if (Xbox.getAnalogHat(RightHatY, i) > 7500 || Xbox.getAnalogHat(RightHatY, i) < -7500) {
						RightJoystickY = 255.0 / 32767 * Xbox.getAnalogHat(RightHatY, i);
					}
				}

				if (Xbox.getButtonPress(UP, i)) {
					LeftJoystickY = 200;
					RightJoystickY = 200;
				}

				if (Xbox.getButtonClick(START, i)) {
					isArcadeDrive = !isArcadeDrive;
				}
				if (Xbox.getButtonClick( B, i)) {
					SwitchOverride = !SwitchOverride;
				}
				if (Xbox.getButtonClick(XBOX, i)) {

					mc.setMotorSpeed(0, 150);
					mc.setMotorSpeed(2, 150);
					mc.setMotorSpeed(1, 150);
					mc.setMotorSpeed(3, 150);

					delay(1000);

					mc.setMotorSpeed(0, 0);
					mc.setMotorSpeed(2, 0);
					mc.setMotorSpeed(1, 0);
					mc.setMotorSpeed(3, 0);
				}

			}
		}
	}
}
