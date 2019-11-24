/*
 * sample_stackless.h
 *
 *  Created on: 24.11.2019
 *      Author: andre
 */

#ifndef SRCEXAMPLE_SAMPLE_STACKLESS_H_
#define SRCEXAMPLE_SAMPLE_STACKLESS_H_

#include "Protothread.h"

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

class Outer : public Protothread
{
  public:
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
