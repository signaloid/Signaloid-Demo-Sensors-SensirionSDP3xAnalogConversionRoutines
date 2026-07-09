/*
 *	Copyright (c) 2026, Signaloid.
 *
 *	Permission is hereby granted, free of charge, to any person obtaining a copy
 *	of this software and associated documentation files (the "Software"), to deal
 *	in the Software without restriction, including without limitation the rights
 *	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *	copies of the Software, and to permit persons to whom the Software is
 *	furnished to do so, subject to the following conditions:
 *
 *	The above copyright notice and this permission notice shall be included in all
 *	copies or substantial portions of the Software.
 *
 *	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *	SOFTWARE.
 */

#include <math.h>
#include <stdbool.h>
#include "kernel.h"

/**
 *	@brief  Implementation of the sign function for distributional values.
 *
 *	@param  arg	: Sign function argument.
 *	@return double	: The result of the sign function.
 */
static double
sign(double arg)
{
	if (arg == 0.0)
	{
		return 0.0;
	}

	return arg / fabs(arg);
}

double
SensirionSDP3x_calculateOutput(uint8_t outputSelect, double *  inputVariables, double *  outputVariables)
{
	double  Vdd     = inputVariables[kSensirionSDP3xInputVariableIndexVdd];
	double  Aout    = inputVariables[kSensirionSDP3xInputVariableIndexAout];
	double  calibratedValue;

	bool calculateAllOutputs = (outputSelect == kSensirionSDP3xOutputVariableIndexCalibratedSensorOutputMax);

	if (calculateAllOutputs ||
	    (outputSelect == kSensirionSDP3xOutputVariableIndexCalibratedSensorOutputSDP36Linear500Pa))
	{
		calibratedValue = kSensirionSDP3xSensorCalibrationConstantSDP36Linear500Pa1 * Aout / Vdd - kSensirionSDP3xSensorCalibrationConstantSDP36Linear500Pa2;
		outputVariables[kSensirionSDP3xOutputVariableIndexCalibratedSensorOutputSDP36Linear500Pa] = calibratedValue;
	}


	if (calculateAllOutputs ||
	    (outputSelect == kSensirionSDP3xOutputVariableIndexCalibratedSensorOutputSDP37Linear125Pa))
	{
		calibratedValue = kSensirionSDP3xSensorCalibrationConstantSDP37Linear125Pa1 * Aout / Vdd - kSensirionSDP3xSensorCalibrationConstantSDP37Linear125Pa2;
		outputVariables[kSensirionSDP3xOutputVariableIndexCalibratedSensorOutputSDP37Linear125Pa] = calibratedValue;
	}

	if (calculateAllOutputs ||
	    (outputSelect == kSensirionSDP3xOutputVariableIndexCalibratedSensorOutputSDP36Sqrt500Pa))
	{
		calibratedValue = sign((Aout / Vdd) - kSensirionSDP3xSensorCalibrationConstantSDP36Sqrt500Pa1) *
		                  pow(
			(Aout / (Vdd * kSensirionSDP3xSensorCalibrationConstantSDP36Sqrt500Pa2)) -
			kSensirionSDP3xSensorCalibrationConstantSDP36Sqrt500Pa3, 2
		                  ) *
		                  kSensirionSDP3xSensorCalibrationConstantSDP36Sqrt500Pa4;
		outputVariables[kSensirionSDP3xOutputVariableIndexCalibratedSensorOutputSDP36Sqrt500Pa] = calibratedValue;
	}

	if (calculateAllOutputs ||
	    (outputSelect == kSensirionSDP3xOutputVariableIndexCalibratedSensorOutputSDP37Sqrt125Pa))
	{
		calibratedValue = sign((Aout / Vdd) - kSensirionSDP3xSensorCalibrationConstantSDP37Sqrt125Pa1) *
		                  pow(
			(Aout / (Vdd * kSensirionSDP3xSensorCalibrationConstantSDP37Sqrt125Pa2)) -
			kSensirionSDP3xSensorCalibrationConstantSDP37Sqrt125Pa3, 2
		                  ) *
		                  kSensirionSDP3xSensorCalibrationConstantSDP37Sqrt125Pa4;
		outputVariables[kSensirionSDP3xOutputVariableIndexCalibratedSensorOutputSDP37Sqrt125Pa] = calibratedValue;
	}

	return calibratedValue;
}
