#pragma once

#include <irrlicht.h>

using namespace irr;

// Represents a trapezoidal fuzzy set.
class TrapezoidalFuzzySet {
private:
	float leftLow, leftHigh, rightHigh, rightLow;
	float minVal, maxVal;

public:
	TrapezoidalFuzzySet(float leftLow, float leftHigh, float rightHigh, float rightLow, float minVal, float maxVal) {
		static_assert(minVal <= maxVal);
		this->leftLow = leftLow;
		this->leftHigh = leftHigh;
		this->rightHigh = rightHigh;
		this->rightLow = rightLow;
		this->minVal = minVal;
		this->maxVal = maxVal;
	}

	float at(float mu) {
		if (mu <= leftLow || mu >= rightLow)
			return minVal;
		else if (leftHigh <= mu && mu <= rightHigh)
			return maxVal;
		else if (leftLow < mu && mu < leftHigh)
			return (maxVal - minVal) / (leftHigh - leftLow) * (mu - leftLow) + minVal;
		else // rightHigh < mu < rightLow
			return (minVal - maxVal) / (rightLow - rightHigh) * (mu - rightHigh) + maxVal;
	}

	float get

};