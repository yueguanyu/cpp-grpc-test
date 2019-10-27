
#include <iostream>
#include <functional>

int main()
{
	std::cout << "Hello";
	// [out = std::ref(std::cout << "Hello ")]() { out.get() << "World\n"; }();
}
