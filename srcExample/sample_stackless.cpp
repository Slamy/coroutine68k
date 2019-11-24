/*
 * sample_stackless.cpp
 *
 *  Created on: 24.11.2019
 *      Author: andre
 */

#include "config.h"

#include <iomanip>
#include <list>
#include <malloc.h>
#include <numeric>
#include <stdio.h>
#include <stdlib.h>
#include <vector>

#include <algorithm> //algorithm must be included latest for bebbo toolchain

#include "CiaMeasure.h"
#include "sample_stackless.h"

Inner in;
Outer co(in);

bool Inner::Run()
{
	PT_BEGIN();

	iteration = 0;

#ifdef MEASURE_TIME
	dontOptimizeMeOut();
#else
	printf("    - %d\n", iteration++);
#endif

	PT_YIELD();

#ifdef MEASURE_TIME
	dontOptimizeMeOut();
#else
	printf("    - %d\n", iteration++);
#endif

	PT_YIELD();

#ifdef MEASURE_TIME
	dontOptimizeMeOut();
#else
	printf("    - %d\n", iteration++);
#endif

	PT_END();
}

bool Outer::Run()
{
	PT_BEGIN();
	iterationOuter = 0;

#ifdef MEASURE_TIME
	dontOptimizeMeOut();
#else
	printf("  - %d\n", iterationOuter++);
#endif

	PT_YIELD();

#ifdef MEASURE_TIME
	dontOptimizeMeOut();
#else
	printf("  - %d\n", iterationOuter++);
#endif

	PT_SPAWN(in);

#ifdef MEASURE_TIME
	dontOptimizeMeOut();
#else
	printf("  - %d\n", iterationOuter++);
#endif

	PT_SPAWN(in);

#ifdef MEASURE_TIME
	dontOptimizeMeOut();
#else
	printf("  - %d\n", iterationOuter++);
#endif

	PT_END();
}

void sample_stackless_protoThread()
{
#ifdef MEASURE_TIME
	while (co.Run())
		;
#else
	printf("%s\n", __func__);
	int iteration = 0;
	printf("- %d\n", iteration++);
	while (co.Run())
	{
		printf("- %d\n", iteration++);
	}
	printf("- %d\n", iteration++);
#endif
}
