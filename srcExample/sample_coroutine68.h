/**
 * @file sample_coroutine68.h
 * @date 24.11.2019
 * @author andre
 */

#ifndef SRCEXAMPLE_SAMPLE_COROUTINE68_H_
#define SRCEXAMPLE_SAMPLE_COROUTINE68_H_

#include "Coroutine68k.h"

/**
 * Very simple coroutine which just does some yielding in top level function and one call deeper.
 */
class TestCoroutine : public CoTask
{
  public:
	void func() override;
	void funcInner();

  private:
};

/**
 * abstract class which contains a boolean signal
 */
class WaitTest : public CoTask
{
  public:
	/// Set to true from outside to give a signal to the coroutine.
	volatile bool gotIt = false;
};

/**
 * Coroutine which waits with macros until it got an external signal
 */
class MacroWaitTest : public WaitTest
{
  public:
	void func() override
	{
		debugLogf("Not Yet\n");
		CO_WAIT_UNTIL(gotIt);
		debugLogf("Finished\n");
	}
};

/**
 * Coroutine which waits with lambdas until it got an external signal
 */
class LambdaWaitTest : public WaitTest
{
  public:
	void func() override
	{
		debugLogf("Not Yet\n");
		coWaitUntil([&] { return gotIt; });
		debugLogf("Finished\n");
	}
};

/**
 * Very simple class which just takes strings and prints them
 */
class TestParserString : public CoParser<std::string>
{
  public:
	void func() override
	{
		debugLogf("TestParserString: I'm alive and ready to take your strings\n");
		for (;;)
		{
			std::string in = source();
			debugLogf("string size is %ld\n", in.size());
		}
	}
};

/// a very stupid candom class
class RandomObject
{
  public:
	/// some random value
	int val;

	/// some random constructor
	RandomObject(int val) : val(val)
	{
		debugLogf("Random object @%lx. val %d\n", (uint32_t)this, val);
	}

	/// some random test function to show that the object is alive
	void test()
	{
		debugLogf("Random test @%lx. val %d\n", (uint32_t)this, val);
	}
};

/**
 * A simple class which takes objects from outside and executes them
 */
class TestParserRandomObject : public CoParser<RandomObject*>
{
  public:
	void func() override
	{
		debugLogf("TestParserRandomObject: I'm alive and ready to take your objects\n");
		for (;;)
		{
			source()->test();
		}
	}
};

/**
 * An example for CoParser which takes in characters and detects a pattern in the text.
 */
class TestParser : public CoParser<char>
{
  private:
	const char* pattern;

  public:
	/// constructs with given text this parser should detect.
	TestParser(const char* text) : pattern(text)
	{
		debugLogf("I will search for %s\n", text);
	}
	void func() override;
};

/**
 * An example CoGenerator which gives out a string.
 */
class TestGenerator : public CoGenerator<char>
{
  private:
	const char* text;

  public:
	/// constructs with given text which is saved for sink()ing
	TestGenerator(const char* text) : text(text)
	{
		debugLogf("I will generate %s\n", text);
	}

	void func() override;
};

/**
 * A very primitive \ref CoTask which just yields() to measure context switching overhead.
 */
class JumpInOutCoroutine : public CoTask
{
  public:
	/**
	 * just does some nop instructions as these are good to find in cpu trace logs.
	 */
	inline void breakpoint()
	{
		asm volatile("nop");
		asm volatile("nop");
		asm volatile("nop");
	}

	void func() override;

	/// an empty function to measure std::bind call time
	void empty()
	{
	}

  private:
};

void sample_coroutine68k_innerOuter();
void sample_coroutine68k_ioWaiting(WaitTest& wait);
void sample_jumpInOut();
void sample_parsers();
void sample_generators();

#endif /* SRCEXAMPLE_SAMPLE_COROUTINE68_H_ */
