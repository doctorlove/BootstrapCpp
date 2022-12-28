#include <iostream>
#include <functional>

// Needs C++14 for std::plus<void> so compile with e.g.
// g++ --std=c++14 hello_world.cpp -o ./hello.out

template<typename T, typename U>
auto simple_plus(T lhs, U rhs) -> decltype(lhs + rhs)
{
    return lhs + rhs;
}

auto main() -> int {
    std::cout << "Hello, world!\n";
    std::plus<int> adder;
    std::cout << adder(1, 1.23) << '\n'; // you possibly get a warning because adder is created to add ints
    std::cout << std::plus<>{}(1, 1.23) << '\n';
    std::cout << simple_plus(1, 1.23) << '\n';

}
