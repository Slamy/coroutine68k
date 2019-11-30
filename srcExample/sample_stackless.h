/**
 * @file sample_stackless.h
 * @date 24.11.2019
 * @author andre
 */

#ifndef SRCEXAMPLE_SAMPLE_STACKLESS_H_
#define SRCEXAMPLE_SAMPLE_STACKLESS_H_

#include "Protothread.h"

/// Coroutine which is spawned for testing by \ref Outer
class Inner : public Protothread
{
  public:
	bool Run();
	virtual ~Inner()
	{
	}

  private:
	int iteration = 0;
};

/// Simple Coroutine which just demonstrates the usage of Protothread
class Outer : public Protothread
{
  public:
	/// creates an outer Protothread with an \ref Inner one to test PT_SPAWN
	Outer(Inner& in) : in(in)
	{
	}

	bool Run();
	virtual ~Outer()
	{
	}

  private:
	int iterationOuter = 0;
	Inner& in;
};

void sample_stackless_protoThread();

#endif /* SRCEXAMPLE_SAMPLE_STACKLESS_H_ */
