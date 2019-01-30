#pragma once
#include "Quadrotor.h"
#include "QuadrotorController.h"

enum QuadrotorTrajectory {
	QT_NONE,
	QT_LOOPING,
	QT_SMALL_CIRCLE,
	QT_BIG_CIRCLE,
	QT_OSCILLATE,
	QT_STABLE_LOW,
	QT_STABLE_MEDIUM,
	QT_STABLE_HIGH,
};

class QuadrotorTrajectoryController {
private:
	Quadrotor* quadrotor;
	QuadrotorController* quadrotorController;

	//void(*currentTrajectory)() = NULL;
	QuadrotorTrajectory currentTrajectory = QT_NONE;

public:


	QuadrotorTrajectoryController(QuadrotorController* controller, Quadrotor* quadrotor):
	quadrotor(quadrotor), quadrotorController(controller){
		this->reset();
	}

	void reset() {
		currentTrajectory = QT_NONE;
		if (quadrotorController)
			quadrotorController->reset();
	}

	void update(float elapsedTime) {
		float params[4];
		switch (currentTrajectory) {
		case QT_STABLE_LOW:
			params[0] = 100; 
			params[1] = params[2] = params[3] = 0.f;
			break;
		case QT_STABLE_MEDIUM:
			params[0] = 1500;
			params[1] = params[2] = params[3] = 0.f;
			break;
		case QT_STABLE_HIGH:
			params[0] = 4000;
			params[1] = params[2] = params[3] = 0.f;
			break;
		case QT_NONE:
			return;
		default:
			return;
		}
		quadrotorController->adjust(params, elapsedTime);
	}
	
	void setTrajectory(QuadrotorTrajectory trajectory) {
		this->currentTrajectory = trajectory;
	}

	void setQuadrotorController(QuadrotorController* controller) {
		this->quadrotorController = controller;
	}


};
