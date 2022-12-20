#include <iostream>
#include <vector>

void generate_triangle()
{
    std::vector<int> data{ 1 };
    for (auto number : data)
    {
        std::cout << number << ' ';
    }
}

int main()
{
    generate_triangle();
}
