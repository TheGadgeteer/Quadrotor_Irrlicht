#pragma once

class PIDController {
private:
	float integral;
	float derivate;
	float lastE;

	float pF, dF, iF, uF;

	// Function to be overwritten by subclasses
	virtual float control(float e, float dE, float iE) {
		return e + dE + iE;
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
		lastE = 0.f;
	}

	float control(float wanted, float in, float elapsedTime) {
		float e = wanted - in;
		integral += e * elapsedTime;
		derivate += (e - lastE) / elapsedTime;
		lastE = e;
		return uF * control(e * pF, derivate * dF, integral * iF);
	}

};