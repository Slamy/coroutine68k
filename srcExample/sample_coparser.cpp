/**
 * @file sample_coparser.cpp
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
#include "sample_coparser.h"

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
	printf("ticks inside parserTest(B) %d\n", time);
	time = measureTime(std::bind(&TestParser::operator(), &parserTest, 'C'));
	printf("ticks inside parserTest(C) %d\n", time);

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
