#pragma once
#include "PIDController.h"
#include <stdio.h>

class FuzzyController : PIDController {
private:
	static float control(float in, float dIn, float iIn,
		float elapsedTime) {
		printf("Fuzzy Controller control called");
		return 0.f;
	}
public:
	FuzzyController() : PIDController() {
	}
};