/*
 *	Copyright (c) 2024-2026, Signaloid.
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

#pragma once

#include <stddef.h>
#include <stdint.h>


/*
 *	These constant values are taken from page 4 of
 *	SDP3x Datasheet, 2024-07-03.
 */
#define kSensirionSDP3xSensorCalibrationConstantSDP36Linear500Pa1   (750)
#define kSensirionSDP3xSensorCalibrationConstantSDP36Linear500Pa2   (150)

#define kSensirionSDP3xSensorCalibrationConstantSDP37Linear125Pa1   (190)
#define kSensirionSDP3xSensorCalibrationConstantSDP37Linear125Pa2   (38)

#define kSensirionSDP3xSensorCalibrationConstantSDP36Sqrt500Pa1 (0.5)
#define kSensirionSDP3xSensorCalibrationConstantSDP36Sqrt500Pa2 (0.4)
#define kSensirionSDP3xSensorCalibrationConstantSDP36Sqrt500Pa3 (1.25)
#define kSensirionSDP3xSensorCalibrationConstantSDP36Sqrt500Pa4 (525)

#define kSensirionSDP3xSensorCalibrationConstantSDP37Sqrt125Pa1 (0.5)
#define kSensirionSDP3xSensorCalibrationConstantSDP37Sqrt125Pa2 (0.4)
#define kSensirionSDP3xSensorCalibrationConstantSDP37Sqrt125Pa3 (1.25)
#define kSensirionSDP3xSensorCalibrationConstantSDP37Sqrt125Pa4 (133)

#define kSensirionSDP3xDefaultInputDistributionAoutUniformDistLow   (1.3)
#define kSensirionSDP3xDefaultInputDistributionAoutUniformDistHigh  (1.7)
#define kSensirionSDP3xDefaultInputDistributionVddUniformDistLow    (3.3)
#define kSensirionSDP3xDefaultInputDistributionVddUniformDistHigh   (3.9)

/*
 *	Input Variables:
 *		kInputVariableIndexAout	: Ratiometric Analog Voltage Value (in Volts)
 *		kInputVariableIndexVdd	: Supply Voltage (in Volts)
 */
typedef enum
{
	kSensirionSDP3xInputVariableIndexAout   = 0,
	kSensirionSDP3xInputVariableIndexVdd    = 1,
	kSensirionSDP3xInputVariableIndexMax,
} SensirionSDP3xInputVariableIndex;

/*
 *	Output Variables:
 *		kOutputVariableIndexCalibratedSensorOutputSDP36Linear500Pa	:
 *			Differential Pressure Output (in Pascal) for the Linear Configuration of SDP36, for 500Pa
 *		kOutputVariableIndexCalibratedSensorOutputSDP37Linear125Pa	:
 *			Differential Pressure Output (in Pascal) for the Linear Configuration of SDP37, for 125Pa
 *		kOutputVariableIndexCalibratedSensorOutputSDP36Sqrt500Pa	:
 *			Differential Pressure Output (in Pascal) for the Square Root Configuration of SDP36, for 500Pa
 *		kOutputVariableIndexCalibratedSensorOutputSDP37Sqrt125Pa	:
 *			Differential Pressure Output (in Pascal) for the Square Root Configuration of SDP37, for 125Pa
 */
typedef enum
{
	kSensirionSDP3xOutputVariableIndexCalibratedSensorOutputSDP36Linear500Pa    = 0,
	kSensirionSDP3xOutputVariableIndexCalibratedSensorOutputSDP37Linear125Pa    = 1,
	kSensirionSDP3xOutputVariableIndexCalibratedSensorOutputSDP36Sqrt500Pa      = 2,
	kSensirionSDP3xOutputVariableIndexCalibratedSensorOutputSDP37Sqrt125Pa      = 3,
	kSensirionSDP3xOutputVariableIndexCalibratedSensorOutputMax,
} SensirionSDP3xOutputVariableIndex;

/**
 *	@brief	Sensor calibration routines for different modes taken from
 *		Sensirion_Differential_Pressure_Datasheet_SDP3x_Analog.pdf, 2024-07-03.
 *		This shared conversion routine is called by both
 *		`sensirionSDP3xUxHw()` (in `sensirion-sdp3x-uxhw.c`)
 *		and `sensirionSDP3xMonteCarlo()` (in
 *		`sensirion-sdp3x-monte-carlo.c`), exactly as GBM's
 *		`geometricBrownianMotionSinglePath()` is called by both of its
 *		mode-specific computational files.
 *
 *	@param	inputVariables		: The array of input variables used in the calculation.
 *	@param	outputVariables		: An array of output variables.
 *					  Writes the result to `outputVariables[outputSelectValue]`.
 *	@return	double			: Returns the distributional value calculated.
 */
double
SensirionSDP3x_calculateOutput(
	uint8_t         outputSelect,
	const double *  inputVariables,
	double *        outputVariables);

/**
 *	@brief	Set the sensor's analog inputs (`Aout`, `Vdd`) via
 *		`UxHwDoubleUniformDist()`. Called once per distributional
 *		evaluation, so it is shared by both `SensirionSDP3xCalculateOutputUxHw()` (a
 *		single call) and `SensirionSDP3xCalculateOutputMonteCarlo()` (once per Monte
 *		Carlo iteration, drawing a fresh compat-RNG sample each time).
 *
 *	@param	inputVariables	: Array of size `kSensirionSDP3xInputVariableIndexMax` to fill.
 */
void
SensirionSDP3xSetInputVariablesViaUxHwCall(double * inputVariables);

/**
 *	@brief	UxHw-mode dispatch kernel. Owns the `inputVariables` array (sized
 *		`kSensirionSDP3xInputVariableIndexMax`), sets the analog inputs via
 *		a single call to `SensirionSDP3xSetInputVariablesViaUxHwCall()`, then hands the
 *		already-sampled array down to `sensirionSDP3xUxHw()`
 *		(in `sensirion-sdp3x-uxhw.c`) for the actual computation. Writes
 *		per-output results into `outputVariables` and the single
 *		distributional calibrated output into `monteCarloOutputSamples[0]`.
 *		Contains no sample loop.
 *
 *		This dispatch kernel reads (via
 *		`SensirionSDP3xSetInputVariablesViaUxHwCall()`) the demo's distributional
 *		configuration macros, which makes it glue code rather than pure
 *		computation; it therefore lives here in `kernel.c`/`kernel.h`
 *		rather than in `sensirion-sdp3x-uxhw.c/.h`.
 *
 *	@param	outputSelect		: Selects which output(s) to calculate;
 *					  `kSensirionSDP3xOutputVariableIndexCalibratedSensorOutputMax`
 *					  calculates all of them. A `size_t`, so that
 *					  the caller passes
 *					  `CommonCommandLineArguments.outputSelect`
 *					  through unnarrowed.
 *	@param	outputVariables		: Array of size `kSensirionSDP3xOutputVariableIndexCalibratedSensorOutputMax` to fill.
 *	@param	monteCarloOutputSamples	: Single-element array for the distributional result.
 *	@return	double			: Returns the value of the selected output (or the last-computed output when all outputs are selected).
 */
double
SensirionSDP3xCalculateOutputUxHw(
	size_t      outputSelect,
	double *    outputVariables,
	double *    monteCarloOutputSamples);

/**
 *	@brief	Monte Carlo dispatch kernel. Owns the `inputVariables` array
 *		(sized `kSensirionSDP3xInputVariableIndexMax`) and hands it, still
 *		unpopulated, down to `sensirionSDP3xMonteCarlo()`
 *		(in `sensirion-sdp3x-monte-carlo.c`), which runs
 *		`numberOfMonteCarloIterations` independent iterations there,
 *		resampling the analog inputs via
 *		`SensirionSDP3xSetInputVariablesViaUxHwCall()` once per iteration (a fresh compat-RNG
 *		sample each time) and filling `monteCarloOutputSamples` with one
 *		sample per iteration, exactly as the original single-loop
 *		implementation did. `outputVariables` is left holding the final
 *		iteration's raw values (not overwritten with the mean).
 *
 *		The mean and variance of `monteCarloOutputSamples`, which
 *		approximate the cost of Monte Carlo's post-processing phase, are
 *		computed by the caller in `main.c` rather than here: they need
 *		`calculateMeanAndVarianceOfDoubleSamples()` from `common.h`, and
 *		`kernel.c` is compiled without that header by the multi-sensor
 *		builds that consume it.
 *
 *		Like `SensirionSDP3xCalculateOutputUxHw()`, this dispatch kernel
 *		sizes the scratch array the computational file works in, which
 *		makes it glue code rather than pure computation; it therefore
 *		lives here rather than in `sensirion-sdp3x-monte-carlo.c/.h`.
 *
 *	@param	outputSelect		: Selects which output(s) to calculate;
 *	@param	numberOfMonteCarloIterations	: Number of Monte Carlo iterations to run.
 *					  `kSensirionSDP3xOutputVariableIndexCalibratedSensorOutputMax`
 *					  calculates all of them. A `size_t`, so that
 *					  the caller passes
 *					  `CommonCommandLineArguments.outputSelect`
 *					  through unnarrowed.
 *	@param	outputVariables		: Array of size `kSensirionSDP3xOutputVariableIndexCalibratedSensorOutputMax` to fill.
 *	@param	monteCarloOutputSamples	: Array of `numberOfMonteCarloIterations` doubles, filled with samples.
 *	@return	double			: Returns the value of the selected output for the final iteration.
 */
double
SensirionSDP3xCalculateOutputMonteCarlo(
	size_t      outputSelect,
	size_t      numberOfMonteCarloIterations,
	double *    outputVariables,
	double *    monteCarloOutputSamples);
