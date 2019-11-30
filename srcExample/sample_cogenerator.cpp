/**
 * @file sample_cogenerator.cpp
 * @date 30.11.2019
 * @author andre
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

#include "sample_cogenerator.h"

void TestGenerator::func()
{
	debugLogf("TestGenerator: I'm alive and ready to push some chars\n");

	const char* ptr = text;
	while (*ptr)
		sink(*(ptr++));
}

/**
 * Provides some example uses of \ref TestGenerator.
 */
void sample_generators()
{
	TestGenerator gen("Hallo");
	uint16_t time;

	time = measureTime([&] {
		gen.init();
		while (gen.hasNext())
		{
			debugLogf("Got %c\n", gen());
		}
	});
	printf("ticks using while loop + gen.hasNext() + gen() %d\n", time);

	time = measureTime([&] {
		for (auto it = gen.begin(); it != gen.end(); ++it)
		{
			debugLogf("Got %c\n", *it);
		}
	});
	printf("ticks using iterator loop %d\n", time);

	time = measureTime([&] {
		for (char out : gen)
		{
			debugLogf("Got %c\n", out);
		}
	});
	printf("ticks using range loop %d\n", time);
}
