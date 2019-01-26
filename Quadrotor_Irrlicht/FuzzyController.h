#pragma once
#include <stdio.h>
#include "TrapezoidalFuzzySet.h"

enum DefuzzificationStrategy {
	FC_DEFUZZI_MOM,
	FC_DEFUZZI_COS,
	FC_DEFUZZI_COA
};

struct FuzzyVar {
	unsigned int numTerms;
	TrapezoidalFuzzySet *terms;
};

struct FuzzyVarTermPair {
	FuzzyVar* var;
	unsigned int termIdx;
};

struct FuzzyRule {
	FuzzyVarTermPair* conditions;
	unsigned int numConditions;

};

class FuzzyPDController {
private:
	int numInput;
	DefuzzificationStrategy defuzStrat;


public:
	FuzzyPDController(int numInputVars, DefuzzificationStrategy strat) :
	numInput(numInputVars), ,defuzStrat(strat){

	}

	float control(float *input) {
		return 0.f;
	}
};