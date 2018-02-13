#include <chip8/Chip8.h>
#include <chrono>


namespace chip8
{

	void Chip8::Tick()
	{
		using clock = std::chrono::high_resolution_clock;

		auto checkpoint = clock::now();
		while(true)
		{
			unsigned counter = 0;
			decltype(checkpoint) now;
			unsigned dt;
			do
			{
				for(unsigned i = 0; i < InstructionsPerStep; ++i)
					Step();

				counter += InstructionsPerStep;

				now = clock::now();
				dt = std::chrono::duration_cast<std::chrono::microseconds>(now - checkpoint).count();
			} while(dt < TimerPeriodMs);
			printf("%u instruction per tick, %g instruction per second\n", counter, counter * 60 / (dt / 1000000.0));
			checkpoint += std::chrono::microseconds(static_cast<unsigned>(TimerPeriodMs));
		}
	}

	void Chip8::Step()
	{ }

}
