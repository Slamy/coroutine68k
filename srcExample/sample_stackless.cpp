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
	debugLogf("    - %d\n", iteration++);
	PT_YIELD();
	debugLogf("    - %d\n", iteration++);
	PT_YIELD();
	debugLogf("    - %d\n", iteration++);

	PT_END();
}

bool Outer::Run()
{
	PT_BEGIN();

	iterationOuter = 0;
	debugLogf("  - %d\n", iterationOuter++);
	PT_YIELD();
	debugLogf("  - %d\n", iterationOuter++);
	PT_SPAWN(in);
	debugLogf("  - %d\n", iterationOuter++);
	PT_SPAWN(in);
	debugLogf("  - %d\n", iterationOuter++);

	PT_END();
}

void sample_stackless_protoThread()
{
	debugLogf("%s\n", __func__);
	int iteration = 0;
	debugLogf("- %d\n", iteration++);
	while (co.Run())
	{
		debugLogf("- %d\n", iteration++);
	}
	debugLogf("- %d\n", iteration++);
}
