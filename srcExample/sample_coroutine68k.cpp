/*
 * sample_coroutine68k.cpp
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
#include "Coroutine68k.h"
#include "config.h"
#include "multiplatform.h"
#include "sample_coroutine68.h"

TestCoroutine cotest;

void TestCoroutine::funcInner()
{
#ifdef MEASURE_TIME
	dontOptimizeMeOut();
#else
	int iteration = 0;
	printf("    - %d\n", iteration++);
#endif

	coYield();

#ifdef MEASURE_TIME
	dontOptimizeMeOut();
#else
	printf("    - %d\n", iteration++);
#endif

	coYield();

#ifdef MEASURE_TIME
	dontOptimizeMeOut();
#else
	printf("    - %d\n", iteration++);
#endif
}

void TestCoroutine::func()
{
#ifdef MEASURE_TIME
	dontOptimizeMeOut();
#else
	int iterationOuter = 0;
	printf("  - %d\n", iterationOuter++);
#endif

	coYield();

#ifdef MEASURE_TIME
	dontOptimizeMeOut();
#else
	printf("  - %d\n", iterationOuter++);
#endif

	funcInner();

#ifdef MEASURE_TIME
	dontOptimizeMeOut();
#else
	printf("  - %d\n", iterationOuter++);
#endif

	funcInner();

#ifdef MEASURE_TIME
	dontOptimizeMeOut();
#else
	printf("  - %d\n", iterationOuter++);
#endif
}

void sample_coroutine68k_innerOuter()
{
	cotest.init();

#ifdef MEASURE_TIME
	while (!cotest.run())
		;
#else
	printf("%s\n", __func__);
	int iteration = 0;
	printf("- %d\n", iteration++);
	while (!cotest.run())
	{
		printf("- %d\n", iteration++);
	}
	printf("- %d\n", iteration++);
	printf("outside finished!\n");
#endif
}

void sample_coroutine68k_ioWaiting(WaitTest& wait)
{
#ifdef MEASURE_TIME
	wait.init();
	int iteration;

	for (iteration = 0; iteration < 7; iteration++)
	{
		if (iteration == 3)
			wait.gotIt = true;
		wait.run();
	}
#else
	printf("%s\n", __func__);

	wait.init();
	int iteration;

	for (iteration = 0; iteration < 7; iteration++)
	{
		if (iteration == 3)
			wait.gotIt = true;
		bool ret = wait.run();
		if (ret)
		{
			printf("T %d\n", iteration);
		}
		else
		{
			printf("F %d\n", iteration);
		}
	}
#endif
}
