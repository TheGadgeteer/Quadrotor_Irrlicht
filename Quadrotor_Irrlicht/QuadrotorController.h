#pragma once
#include "PIDController.h"
#include "FuzzyController.h"
#include "Quadrotor.h"

class QuadrotorController {
private:
	PIDController heightController;
	PIDController rollController;
	PIDController pitchController;
	PIDController yawController;

	Quadrotor* quadrotor;

	float lastErrors[4];
	float derivates[4];
	float integrals[4];
public:
	QuadrotorController(PIDController height, PIDController roll,
		PIDController pitch, PIDController yaw, Quadrotor* quadrotor) {
		this->heightController = height;
		this->rollController = roll;
		this->pitchController = pitch;
		this->yawController = yaw;
		this->quadrotor = quadrotor;
		reset();
	}

	void reset() {
		for (int i = 0; i < 4; ++i) {
			lastErrors[i] = 0.f;
			derivates[i] = 0.f;
			integrals[i] = 0.f;
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
			integrals[i] += errors[i] * elapsedTime;
			derivates[i] = (errors[i] - lastErrors[i]) / elapsedTime;
			lastErrors[i] = errors[i];
		}
		float uHeight = heightController.control(errors[0], derivates[0], integrals[0]);
		float uRoll = rollController.control(errors[1], derivates[1], integrals[1]);
		float uPitch = pitchController.control(errors[2], derivates[2], integrals[2]);
		float uYaw = yawController.control(errors[3], derivates[3], integrals[3]);
		
		float outSpeeds[4];

		outSpeeds[0] = uHeight - uRoll + uPitch - uYaw;
		outSpeeds[1] = uHeight + uRoll + uPitch + uYaw;
		outSpeeds[2] = uHeight - uRoll - uPitch + uYaw;
		outSpeeds[3] = uHeight + uRoll - uPitch - uYaw;

		quadrotor->setMotorSpeed(outSpeeds);
	}


};