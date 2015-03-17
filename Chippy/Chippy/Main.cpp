#include <iostream>
#include "chip8.h"
using namespace std;

int main(int argc, char *argv[])
{
	chip8 emulator;
	emulator.run();
	return 0;
}