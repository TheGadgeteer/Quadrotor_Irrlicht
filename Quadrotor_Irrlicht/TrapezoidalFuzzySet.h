#pragma once

#include <assert.h>
#include <float.h>

// Represents a trapezoidal fuzzy set.
class TrapezoidalFuzzySet {
private:
	float leftLow, leftHigh, rightHigh, rightLow;
	float minVal, maxVal;
	float minDefinedX, maxDefinedX;
public:
	TrapezoidalFuzzySet(float leftLow, float leftHigh, float rightHigh, float rightLow, float minVal, float maxVal) {
		assert(minVal <= maxVal);
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

	// val must be >= minVal and <= maxVal
	float inverseAt_min(float val) {
		if (val == maxVal)
			return leftHigh;
		else if (val == minVal)
			return -FLT_MAX;
		else
			return (val - minVal) * (leftHigh - leftLow) / (maxVal - minVal) + leftLow;
	}

	// val must be >= minVal and <= maxVal
	float inverseAt_max(float val) {
		if (val == maxVal)
			return rightHigh;
		else if (val == minVal)
			return FLT_MAX;
		else
			return (val - maxVal) * (rightLow - rightHigh) / (minVal - maxVal) + rightHigh;
	}

	float getMaxVal() {
		return maxVal;
	}
	float getMinVal() {
		return minVal;
	}

};