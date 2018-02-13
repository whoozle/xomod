#include <chip8/Chip8.h>

int main(int argc, char **argv)
{
	using namespace chip8;
	Chip8 chip;

	while(true)
		chip.Tick();
	return 0;
}
