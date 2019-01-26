#pragma once
#include "PDController.h"

class PIDController : PDController {
private:
	float iF;

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

	virtual float control(float e, float dE) {
		return control(e, dE, 0);
	}

	virtual float control(float e, float dE, float iE) {	
		return uF * controlInner(e * pF, dE * dF, iE * iF);
	}
	

};