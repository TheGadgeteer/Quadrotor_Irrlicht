#pragma once
#include "PDController.h"
#include "Quadrotor.h"

class QuadrotorController {
private:
	PDController heightController;
	PDController rollpitchController;
	PDController yawController;

	Quadrotor* quadrotor;

	float lastErrors[4];
	float derivates[4];
public:
	QuadrotorController(PDController height, PDController rollpitch, PDController yaw, Quadrotor* quadrotor) :
		heightController(height), rollpitchController(rollpitch), yawController(yaw),
		quadrotor(quadrotor){
		reset();
	}

	void reset() {
		for (int i = 0; i < 4; ++i) {
			lastErrors[i] = 0.f;
			derivates[i] = 0.f;
		}
	}

	// inputParams has 4 elements; 0 is the desired height, 1 the desired roll and so on.
	void adjust(float* inputParams, float elapsedTime) {
		// Calculate the error and its derivate and integral
		float errors[4];
		// In the engine's coordinate system, the Z and Y - axis are swapped
		errors[0] = inputParams[0] - quadrotor->getAbsolutePosition().Y;
		errors[1] = inputParams[1] - quadrotor->getRotation().X;
		errors[2] = inputParams[2] - quadrotor->getRotation().Z;
		errors[3] = inputParams[3] - quadrotor->getRotation().Y;

		for (int i = 0; i < 4; ++i) {
			derivates[i] = (errors[i] - lastErrors[i]) / elapsedTime;
			lastErrors[i] = errors[i];
		}
		float uHeight = heightController.control(errors[0], derivates[0]);
		float uRoll = rollpitchController.control(errors[1], derivates[1]);
		float uPitch = rollpitchController.control(errors[2], derivates[2]);
		float uYaw = yawController.control(errors[3], derivates[3]);
		
		float outSpeeds[4];

		outSpeeds[0] = uHeight - uRoll + uPitch - uYaw;
		outSpeeds[1] = uHeight + uRoll + uPitch + uYaw;
		outSpeeds[2] = uHeight - uRoll - uPitch + uYaw;
		outSpeeds[3] = uHeight + uRoll - uPitch - uYaw;

		quadrotor->setMotorSpeed(outSpeeds);
	}


};