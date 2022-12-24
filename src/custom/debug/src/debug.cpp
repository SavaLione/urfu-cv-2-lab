#include "debug.h"

#include <cstddef>


std::size_t some(std::size_t s)
{
	return (s + 16);
}

int main()
{
	std::size_t s = 0;

	s = some(s);

	std::size_t a = some(16);
	std::size_t b = some(32);
	std::size_t c = some(64);
	std::size_t d = some(128);
	
	return 0;
}