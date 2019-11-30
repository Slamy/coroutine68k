/**
 * @file sample_coroutine68k.cpp
 * @date 24.11.2019
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
#include "sample_coroutine68.h"

/// global construction of TestCoroutine to avoid construction having an influence on runtime.
TestCoroutine cotest;

/**
 * A function which is not the top-level of the coroutine but is still able do coYield()
 * because this is a stackful coroutine.
 */
void TestCoroutine::funcInner()
{
	int iteration = 0;
	debugLogf("    - %d\n", iteration++);
	coYield();
	debugLogf("    - %d\n", iteration++);
	coYield();
	debugLogf("    - %d\n", iteration++);
}

void TestCoroutine::func()
{
	int iterationOuter = 0;
	debugLogf("  - %d\n", iterationOuter++);
	coYield();
	debugLogf("  - %d\n", iterationOuter++);
	funcInner();
	debugLogf("  - %d\n", iterationOuter++);
	funcInner();
	debugLogf("  - %d\n", iterationOuter++);
}

void JumpInOutCoroutine::func()
{
	for (;;)
	{
		coYield();
		breakpoint();
		coYield();
		breakpoint();
		coYield();
		breakpoint();
		coYield();
		breakpoint();
		coYield();
		breakpoint();
	}
	coYield();
}

void TestGenerator::func()
{
	debugLogf("TestGenerator: I'm alive and ready to push some chars\n");

	const char* ptr = text;
	while (*ptr)
		sink(*(ptr++));
}

void TestParser::func()
{
	const char* ptr = pattern;
	debugLogf("TestParser: I'm alive and ready to take your tokens\n");
	for (;;)
	{
		char in = source();
		if (in == *ptr)
		{
			debugLogf("Accepted %c\n", in);
			ptr++;
			if (*ptr == '\0')
			{
				debugLogf("Detected target string!\n");
				ptr = pattern;
			}
		}
		else
		{
			debugLogf("Denied %c\n", in);
			ptr = pattern;
		}
	}
}

/**
 * executes \ref cotest until finished. used as way to measure performance against
 * \ref sample_stackless_protoThread
 */
void sample_coroutine68k_innerOuter()
{
	cotest.init();

	debugLogf("%s\n", __func__);
	int iteration = 0;
	debugLogf("- %d\n", iteration++);
	while (!cotest())
	{
		debugLogf("- %d\n", iteration++);
	}
	debugLogf("- %d\n", iteration++);
	debugLogf("outside finished!\n");
}

/**
 * Executes given WaitTest coroutine for 3 times and the gives it a signal to react to.
 * @param wait	Coroutine which implements WaitTest
 */
void sample_coroutine68k_ioWaiting(WaitTest& wait)
{
	debugLogf("%s\n", __func__);

	wait.init();
	int iteration;

	for (iteration = 0; iteration < 7; iteration++)
	{
		if (iteration == 3)
			wait.gotIt = true;
		bool ret = wait();
		if (ret)
		{
			debugLogf("T %d\n", iteration);
		}
		else
		{
			debugLogf("F %d\n", iteration);
		}
	}
}

/**
 * executes \ref JumpInOutCoroutine and measures round trip time and therefore context switching
 * overhead.
 */
void sample_jumpInOut()
{
	uint16_t time;

	JumpInOutCoroutine jumpInOut;
	jumpInOut.init();

	jumpInOut();
	jumpInOut();
	jumpInOut();

	time = measureTime(std::bind(&JumpInOutCoroutine::operator(), &jumpInOut));
	printf("ticks inside jumpInOut %d\n", time);
	time = measureTime(std::bind(&JumpInOutCoroutine::operator(), &jumpInOut));
	printf("ticks inside jumpInOut %d\n", time);
	time = measureTime(std::bind(&JumpInOutCoroutine::empty, &jumpInOut));
	printf("ticks inside jumpInOut std::bind empty %d\n", time);
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

/**
 * Executes some parsers as examples
 */
void sample_parsers()
{
	uint16_t time;

	TestParser parserTest("ABC");
	parserTest.init();

	parserTest('A');
	parserTest('x');
	parserTest('A');

	time = measureTime(std::bind(&TestParser::operator(), &parserTest, 'B'));
	printf("ticks inside parserTest %d\n", time);
	time = measureTime(std::bind(&TestParser::operator(), &parserTest, 'C'));
	printf("ticks inside parserTest %d\n", time);

	parserTest('B');
	parserTest('C');
	parserTest('x');

	TestParserString parserTest2;
	parserTest2.init();
	parserTest2(std::string("Barf"));
	parserTest2(std::string("Barf2!"));

	TestParserRandomObject parserTest3;
	parserTest3.init();
	RandomObject r(42);
	parserTest3(&r);
}
