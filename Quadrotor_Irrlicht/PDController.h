#pragma once

class PDController {

protected:
	float pF, dF, uF;

	// Function to be overwritten by subclasses
	virtual float controlInner(float e, float dE) {
		return e + dE;
	}

public:
	PDController(float pF = 1.f, float dF = 1.f, float uF = 1.f) {
		this->pF = pF;
		this->dF = dF;
		this->uF = uF;
	}


	virtual float control(float e, float dE) {
		return uF * controlInner(e * pF, dE * dF);
	}


};