#pragma once
#include <stdio.h>
#include "TrapezoidalFuzzySet.h"
#include "PDController.h"
#include <stdlib.h>

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
	FuzzyVarTermPair* outputs;
	unsigned int numOutputs;
};

class FuzzyPDController : PDController {
private:
	FuzzyVar *inVars, *outVar;
	unsigned int numInputVars;
	DefuzzificationStrategy defuzStrat;
	FuzzyRule* rules;
	unsigned int numFuzzyRules;

public:
	FuzzyPDController(FuzzyVar *inVars, unsigned int numInputVars, FuzzyVar *outVar, FuzzyRule rules[], int numRules, DefuzzificationStrategy strat) :
	numInputVars(numInputVars), inVars(inVars), outVar(outVar), rules(rules), numFuzzyRules(numRules), defuzStrat(strat){

	}

	float control(float *input) {
		// Fuzzification and Rule evaluation
		float *outTermCap = new float[outVar->numTerms];
		memset(outTermCap, 0, sizeof(float) * outVar->numTerms);

		for (unsigned int i = 0; i < numFuzzyRules; ++i) {
			float ruleConseq = 1.f;
			for (unsigned int condIdx = 0; condIdx < rules[i].numConditions; ++condIdx) {
				FuzzyVarTermPair* cond = &rules[i].conditions[condIdx];
				unsigned int inputIdx = 0;
				// Find corresponding inputIdx to cond->var
				for (; cond->var != &inVars[inputIdx] && inputIdx < numInputVars; inputIdx++)
					;
				ruleConseq = fmin(ruleConseq, cond->var->terms[cond->termIdx].at(input[inputIdx]));
			}
			unsigned int outTermIdx = rules[i].outputs[0].termIdx;
			outTermCap[outTermIdx] = fmax(outTermCap[outTermIdx], ruleConseq);
		}
		// Defuzzification
		float outControlVal;
		switch (defuzStrat) {
			case FC_DEFUZZI_COS:

				break;

			case FC_DEFUZZI_MOM:
				float leftMax = -FLT_MAX, rightMax = FLT_MAX, max = 0.f;
				for (unsigned int termIdx = 0; termIdx < outVar->numTerms; ++termIdx) {
					float termMaxVal = fmin(outVar->terms[termIdx].getMaxVal(), outTermCap[termIdx]);
					if (termMaxVal >= max) {
						float maxCoord1 = outVar->terms[termIdx].inverseAt_min(termMaxVal);
						float maxCoord2 = outVar->terms[termIdx].inverseAt_max(termMaxVal);
						if (leftMax > maxCoord1 || max < termMaxVal)
							leftMax = maxCoord1;
						if (rightMax < maxCoord2 || max < termMaxVal)
							rightMax = maxCoord2;
					}
				}
				outControlVal = (leftMax + rightMax) / 2;
				break;
		}

		delete outTermCap;
		return 0.f;
	}
};