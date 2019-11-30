/**
 * @file sample_coparser.h
 * @date 30.11.2019
 * @author andre
 */

#ifndef SRCEXAMPLE_SAMPLE_COPARSER_H_
#define SRCEXAMPLE_SAMPLE_COPARSER_H_

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

void sample_parsers();

#endif /* SRCEXAMPLE_SAMPLE_COPARSER_H_ */
