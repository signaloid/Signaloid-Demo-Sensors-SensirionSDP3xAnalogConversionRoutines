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

#include "kernel.h"
#include "sensirion-sdp3x-uxhw.h"

double
sensirionSDP3xUxHw(
	uint8_t         outputSelect,
	const double *  inputVariables,
	double *        outputVariables)
{
	/*
	 *	`inputVariables` has already been sampled by the caller (see
	 *	`SensirionSDP3xCalculateOutputUxHw()` in `kernel.c`); this function only runs the
	 *	shared conversion routine on it, so it takes no command-line-arguments
	 *	struct pointer and reads none of this demo's config macros.
	 */
	return SensirionSDP3x_calculateOutput(outputSelect, inputVariables, outputVariables);
}
