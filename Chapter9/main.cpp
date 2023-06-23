#include <algorithm>
#include <cassert>
#include <execution>
#include <iostream>
#include <iterator>
#include <map>
#include <numeric>
#include <ranges>
#include <utility>
#include <vector>

// Listing 9.1 Make the first few triangle numbers
std::vector<int> make_triangle_numbers(int count)
{
	std::vector<int> numbers(count);
	std::iota(numbers.begin(), numbers.end(), 1);
	std::partial_sum(numbers.begin(), numbers.end(), numbers.begin());
	return numbers;
}

// Listing 9.2 and 9.3 Test our triangle numbers
void check_properties()
{
	const int count = 20;
	const auto triangle_numbers = make_triangle_numbers(count);
	std::vector<int> diffs(count);
	std::adjacent_difference(triangle_numbers.begin(), triangle_numbers.end(), diffs.begin());
	std::vector<int> numbers(count);
	std::iota(numbers.begin(), numbers.end(), 1);
	assert(numbers == diffs);
	std::adjacent_difference(diffs.begin(), diffs.end(), diffs.begin());
	assert(std::all_of(diffs.begin(), diffs.end(), [](int x) { return x == 1; }));
	// parallel request
	assert(std::count(std::execution::par, diffs.begin(), diffs.end(), 1) == count);

	// Check closed form n * (n+1)/2 // TODO an algo rather than an old skool for loop?
	for (size_t i=0; i< triangle_numbers.size(); ++i)
	{
		const int n = i + 1;
		assert(triangle_numbers[i] == n*(n+1)/2);
	}
}

void demo_further_properties()
{
	const int count = 20;
	const auto triangle_numbers = make_triangle_numbers(count);
	std::vector<char> odd_or_even;
	std::ranges::transform(triangle_numbers,
		std::back_inserter(odd_or_even),
		[](int i) { return i % 2 ? '.' : '*'; });
	std::ranges::copy(odd_or_even, std::ostream_iterator<char>(std::cout, ""));
	std::cout << '\n';

	std::map<int, int> last_digits;
	//for (int number: triangle_numbers)
	//{
	//	++last_digits[number % 10];
	//}
	std::generate_n(std::inserter(last_digits, last_digits.begin()), 
		10, // how many digits 
		[n=0, triangle_numbers]() mutable  // TODO can I do a const & in the capture... is trasnfrom clearer?
		{ 
			auto key_value = std::make_pair(n, std::count_if(triangle_numbers.begin(), triangle_numbers.end(), [n](int i) { return i % 10 == n; }));
			++n;
			return key_value;
		}
	);
	for (const auto& [key, value] : last_digits)
	{
		std::cout << key << " : " << value << '\n';
	}
}

int main()
{
	check_properties();
	demo_further_properties();
}