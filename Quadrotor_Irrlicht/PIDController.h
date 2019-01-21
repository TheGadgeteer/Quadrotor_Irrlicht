#pragma once

class PIDController {
private:
	float integral;
	float derivate;
	float lastIn;

	float pF, dF, iF, uF;

	static float control(float in, float dIn, float iIn,
		float elapsedTime) {
		return in + dIn + iIn;
	}

public:
	PIDController(float pF = 1.f, float dF = 1.f, float iF = 1.f, float uF = 1.f) {
		this->pF = pF;
		this->dF = dF;
		this->iF = iF;
		this->uF = uF;
		reset();
	}

	void reset() {
		integral = 0.f;
		derivate = 0.f;
		lastIn = 0.f;
	}

	float control(float in, float elapsedTime) {
		integral += in * elapsedTime;
		derivate += (in - lastIn) / elapsedTime;
		lastIn = in;
		return uF * control(in * pF, derivate * dF, integral * iF, elapsedTime);
	}

};