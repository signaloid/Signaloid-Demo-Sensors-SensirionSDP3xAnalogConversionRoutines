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

/**
 *	@brief	Monte Carlo computational kernel. Runs `numberOfMonteCarloIterations`
 *		independent iterations, each of which resamples the analog inputs
 *		via `SensirionSDP3xSetInputVariablesViaUxHwCall()` (declared in `kernel.h`; drawing a
 *		fresh compat-RNG sample per iteration) and evaluates
 *		`SensirionSDP3x_calculateOutput()` (also declared in `kernel.h`)
 *		once. Fills `monteCarloOutputSamples` (length
 *		`numberOfMonteCarloIterations`) with one sample per iteration, and
 *		leaves `outputVariables` holding the values from the final
 *		iteration. Takes no command-line-arguments struct pointer and reads
 *		none of this demo's config macros itself, so it can be imported as
 *		a purely computational unit; it merely calls the two
 *		`kernel.h`-declared functions above, exactly as GBM's
 *		`geometricBrownianMotionMonteCarlo()` calls its shared single-path
 *		function once per iteration.
 *
 *	@param	outputSelect			: Selects which output(s) to calculate; `kSensirionSDP3xOutputVariableIndexCalibratedSensorOutputMax` calculates all of them.
 *	@param	inputVariables			: Array of size `kSensirionSDP3xInputVariableIndexMax`, resampled every iteration.
 *	@param	outputVariables			: Array of size `kSensirionSDP3xOutputVariableIndexCalibratedSensorOutputMax` to fill.
 *	@param	monteCarloOutputSamples		: Array of `numberOfMonteCarloIterations` doubles, filled with samples.
 *	@param	numberOfMonteCarloIterations	: Number of Monte Carlo iterations to run.
 *	@return	double				: Returns the value of the selected output for the final iteration.
 */
double
sensirionSDP3xMonteCarlo(
	uint8_t     outputSelect,
	double *    inputVariables,
	double *    outputVariables,
	double *    monteCarloOutputSamples,
	size_t      numberOfMonteCarloIterations);
