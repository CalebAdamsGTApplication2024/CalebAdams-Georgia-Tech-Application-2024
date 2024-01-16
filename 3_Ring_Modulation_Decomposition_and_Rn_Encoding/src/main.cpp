
#include "main.h"

int main()
{
	Encoder encoder = Encoder::Encoder();

	for (auto i = 0; i < 1000000000; ++i)
	{
		encoder.test1();
		//encoder.test2();
		//encoder.rmdQuantizationTest();
	}

	return 0;
}