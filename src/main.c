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

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include "utilities.h"
#include "kernel.h"
#include "common.h"

#ifdef NO_OS_AVAILABLE

void
returnZeroNoOS(void);
#endif

int
main(int argc, char *  argv[])
{
	CommandLineArguments arguments = { 0 };

	double *                    monteCarloOutputSamples = NULL;
	clock_t                     start = 0;
	clock_t                     end;
	double                      cpuTimeUsedSeconds = 0.0;
	double                      outputVariables[kSensirionSDP3xOutputVariableIndexCalibratedSensorOutputMax] = { 0 };
	const char *                outputVariableNames[kSensirionSDP3xOutputVariableIndexCalibratedSensorOutputMax] = {
		"outputSDP36Linear",
		"outputSDP37Linear",
		"outputSDP36Square",
		"outputSDP37Square"
	};
	const char *                outputVariableDescriptions[kSensirionSDP3xOutputVariableIndexCalibratedSensorOutputMax] = {
		"Calibrated Sensor Output SDP36 Linear",
		"Calibrated Sensor Output SDP37 Linear",
		"Calibrated Sensor Output SDP36 Square",
		"Calibrated Sensor Output SDP37 Square"
	};
	kOutputVariableTypeIndex    outputVariableTypes[kSensirionSDP3xOutputVariableIndexCalibratedSensorOutputMax] = {
		[kSensirionSDP3xOutputVariableIndexCalibratedSensorOutputSDP36Linear500Pa]  = kOutputVariableTypeDistribution,
		[kSensirionSDP3xOutputVariableIndexCalibratedSensorOutputSDP37Linear125Pa]  = kOutputVariableTypeDistribution,
		[kSensirionSDP3xOutputVariableIndexCalibratedSensorOutputSDP36Sqrt500Pa]    = kOutputVariableTypeDistribution,
		[kSensirionSDP3xOutputVariableIndexCalibratedSensorOutputSDP37Sqrt125Pa]    = kOutputVariableTypeDistribution,
	};
	const char *                applicationDescription = "Sensirion SDP3x Analog Conversion Routines";

	/*
	 *	Get command line arguments.
	 */
	if (getCommandLineArguments(argc, argv, &arguments))
	{
		return kCommonConstantReturnTypeError;
	}

	/*
	 *	Allocate memory for the Monte Carlo samples array. This is always
	 *	allocated, even outside Monte Carlo mode, because
	 *	`SensirionSDP3xCalculateOutputUxHw()` writes its single distributional result to
	 *	`monteCarloOutputSamples[0]`.
	 */
	monteCarloOutputSamples = (double *) checkedMalloc(
		(arguments.common.numberOfMonteCarloIterations > 0 ? arguments.common.numberOfMonteCarloIterations : 1) * sizeof(double),
		__FILE__,
		__LINE__
	);

	const bool isScalarOutputSelected = isSelectedOutputScalar(
		&arguments.common,
		outputVariableTypes,
		kSensirionSDP3xOutputVariableIndexCalibratedSensorOutputMax
	);

	/*
	 *	Start timing.
	 */
	if (arguments.common.isTimingEnabled)
	{
		start = clock();
	}

	/*
	 *	Dispatch to the mode-specific kernel. The Monte Carlo loop lives
	 *	inside `SensirionSDP3xCalculateOutputMonteCarlo`; UxHw mode runs a single
	 *	distributional evaluation inside `SensirionSDP3xCalculateOutputUxHw`.
	 *	Both take the fields they need as scalars, so this is the only place
	 *	that unpacks `CommandLineArguments` for the kernel.
	 */
	if (arguments.common.isMonteCarloMode)
	{
		(void) SensirionSDP3xCalculateOutputMonteCarlo(
			arguments.common.outputSelect,
			arguments.common.numberOfMonteCarloIterations,
			outputVariables,
			monteCarloOutputSamples
		);

		/*
		 *	Approximate the cost of the third phase of Monte Carlo
		 *	(post-processing), by calculating the mean and variance.
		 *	Matches the original implementation: `outputVariables` is
		 *	left holding the final iteration's raw values (not
		 *	overwritten with the mean), and the result is not otherwise
		 *	read. This lives here rather than in the dispatch kernel
		 *	because it needs `common.h`, which `kernel.c` is compiled
		 *	without by the multi-sensor builds that consume it.
		 */
		if (!isScalarOutputSelected)
		{
			(void) calculateMeanAndVarianceOfDoubleSamples(
				monteCarloOutputSamples,
				arguments.common.numberOfMonteCarloIterations
			);
		}
	}
	else
	{
		(void) SensirionSDP3xCalculateOutputUxHw(
			arguments.common.outputSelect,
			outputVariables,
			monteCarloOutputSamples
		);
	}

	/*
	 *	Stop timing.
	 */
	if (arguments.common.isTimingEnabled)
	{
		end                 = clock();
		cpuTimeUsedSeconds  = ((double) (end - start)) / CLOCKS_PER_SEC;
	}

	/*
	 *	Print the results (either in JSON or standard output format).
	 */
	if (arguments.common.isOutputJSONMode)
	{
		printJSONFormattedOutput(
			&arguments.common,
			monteCarloOutputSamples,
			outputVariables,
			outputVariableDescriptions,
			kSensirionSDP3xOutputVariableIndexCalibratedSensorOutputMax,
			applicationDescription,
			outputVariableTypes
		);
	}
	else
	{
		printHumanConsumableOutput(
			&arguments.common,
			kSensirionSDP3xOutputVariableIndexCalibratedSensorOutputMax,
			outputVariables,
			outputVariableNames,
			outputVariableDescriptions,
			monteCarloOutputSamples,
			outputVariableTypes
		);
	}

	/*
	 *	Print timing result.
	 */
	if (arguments.common.isTimingEnabled)
	{
		printf("\nCPU time used: %lf seconds\n", cpuTimeUsedSeconds);
	}

	/*
	 *	Write output data.
	 */
	if (arguments.common.isWriteToFileEnabled)
	{
		if (writeOutputDoubleDistributionsToCSV(
				arguments.common.outputFilePath,
				outputVariables,
				outputVariableNames,
				kSensirionSDP3xOutputVariableIndexCalibratedSensorOutputMax
		))
		{
			return kCommonConstantReturnTypeError;
		}
	}

	/*
	 *	Save Monte carlo outputs in an output file.
	 *	Free dynamically-allocated memory.
	 */
	if (arguments.common.isMonteCarloMode)
	{
		saveMonteCarloDoubleDataToDataDotOutFile(
			monteCarloOutputSamples, (uint64_t) (cpuTimeUsedSeconds * 1000000),
			isScalarOutputSelected ? 1 : arguments.common.numberOfMonteCarloIterations
		);
	}

	free(monteCarloOutputSamples);

#ifdef NO_OS_AVAILABLE
	returnZeroNoOS();
#else

	return 0;

#endif
}
