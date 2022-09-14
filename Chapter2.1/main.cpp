#include <iostream>
#include <vector>

void triangle()
{
	auto data = std::vector{ 1 };
	for (const auto& number : data)
	{
		std::cout << number << ' ';
	}

	for (auto p = data.begin(); p!=data.end(); ++p)
	{
		std::cout << *p << ' ';
	}
}

int main()
{
	triangle();
}