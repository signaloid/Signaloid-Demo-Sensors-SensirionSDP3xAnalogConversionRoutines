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

#include <stdint.h>

/**
 *	@brief	UxHw-mode computational kernel. Performs a single evaluation of
 *		the sensor's calibration routines, via `SensirionSDP3x_calculateOutput()`
 *		(declared in `kernel.h`), on an already-sampled `inputVariables`
 *		array. Takes no command-line-arguments struct pointer and reads
 *		none of this demo's config macros, so it can be imported as a
 *		purely computational unit; on real UxHw hardware, distributional
 *		arithmetic in the underlying `double` type is what turns this
 *		single evaluation into a fully distributional result. Writes
 *		per-output results into `outputVariables`.
 *
 *	@param	outputSelect	: Selects which output(s) to calculate; `kSensirionSDP3xOutputVariableIndexCalibratedSensorOutputMax` calculates all of them.
 *	@param	inputVariables	: Array of size `kSensirionSDP3xInputVariableIndexMax`, already populated with sampled input values.
 *	@param	outputVariables	: Array of size `kSensirionSDP3xOutputVariableIndexCalibratedSensorOutputMax` to fill.
 *	@return	double		: Returns the value of the selected output (or the last-computed output when all outputs are selected).
 */
double
sensirionSDP3xUxHw(
	uint8_t         outputSelect,
	const double *  inputVariables,
	double *        outputVariables);
