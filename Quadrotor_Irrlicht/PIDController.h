#pragma once

class PIDController {
private:
	float pF, dF, iF, uF;

	// Function to be overwritten by subclasses
	virtual float controlInner(float e, float dE, float iE) {
		return e + dE + iE;
	}

public:
	PIDController(float pF = 1.f, float dF = 1.f, float iF = 1.f, float uF = 1.f) {
		this->pF = pF;
		this->dF = dF;
		this->iF = iF;
		this->uF = uF;
	}


	float control(float e, float dE, float iE) {	
		return uF * controlInner(e * pF, dE * dF, iE * iF);
	}
	

};