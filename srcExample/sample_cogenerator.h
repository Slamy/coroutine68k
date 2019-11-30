/**
 * @file sample_cogenerator.h
 * @date 30.11.2019
 * @author andre
 */

#ifndef SRCEXAMPLE_SAMPLE_COGENERATOR_H_
#define SRCEXAMPLE_SAMPLE_COGENERATOR_H_

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

void sample_generators();

#endif /* SRCEXAMPLE_SAMPLE_COGENERATOR_H_ */
