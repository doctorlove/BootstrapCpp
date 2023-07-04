#include <algorithm>
#include <cassert>
#include <execution>
#include <format>
#include <iostream>
#include <iterator>
#include <map>
#include <numeric>
#include <random>
#include <ranges>
#include <set>
#include <string>
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
	// TODO https://stackoverflow.com/questions/65977688/rangesviewsgroup-by-like-function-applying-predicate-to-consecutive-elements
	// views::chunk_by
	// https://en.cppreference.com/w/cpp/ranges/chunk_by_view
	for (const auto& [key, value] : last_digits)
	{
		std::cout << key << " : " << value << '\n';
	}
}

//std::any_of

int calculate_payout_v1(int left, int middle, int right)
{
	const int left_unit = left % 10;
	const int middle_unit = middle % 10;
	const int right_unit = right % 10;
	int payout = 0;
	if (left_unit == middle_unit && middle_unit == right_unit)
	{
		payout = 2;
	}
	else if (left_unit == middle_unit || middle_unit == right_unit || left_unit == right_unit)
	{
		payout = 1;
	}
	return payout;
}

std::map<int, size_t> frequencies(std::convertible_to<int> auto ...numbers)
{
	std::map<int, size_t> counter{};
	for (int i : { int(numbers)... })
		counter[i]++;
	return counter;
}

int calculate_payout(int left, int middle, int right)
{
	std::map<int, size_t> counter=frequencies(left, middle, right);
	// for loop first then dots:
	//for (int number : {left, middle, right})
	//{
	//	++counter[number % 10];
	//}
	// TODO compare with max_element next:
	//if (counter.size()==1)
	//{
	//	if (counter[3] == 3 || counter[8] == 3)
	//	{
	//		return 4;
	//	}
	//	return 2;
	//}
	//else if (counter.size() == 2)
	//{
	//	if (counter[3] == 2 || counter[8] == 2)
	//	{
	//		return 2;
	//	}
	//	return 1;
	//}
	//return 0;
	// TODO can we constexpr this?
	// TODO ranges version and mention niebloids
	// https://en.cppreference.com/w/cpp/algorithm/ranges/max_element
	/*
	* The function-like entities described on this page are niebloids, that is:
	* Explicit template argument lists cannot be specified when calling any of them.
	* None of them are visible to argument-dependent lookup.
	* When any of them are found by normal unqualified lookup as the name to the left of the function-call operator, 
	*	argument-dependent lookup is inhibited.
	*/
	auto it = std::max_element(counter.begin(), counter.end(),
		[](auto it1, auto it2) { return it1.second < it2.second; });
	// can this ever be end?
	// what if two match?
	if (it != counter.end())
	{
		int digit = it->first;
		size_t count = it->second;
		constexpr int value[] = {0, 0, 1, 2};
		auto pay = ((digit == 8 || digit == 3) ? 2 : 1) * value[count];
		return pay;
	}
	return 0;
}


void triangle_machine_spins_only()
{
	constexpr int numbers = 20;
	constexpr size_t number_of_reels = 3u;
	std::vector<std::vector<int>> reels(number_of_reels, make_triangle_numbers(numbers));
	std::random_device rd;
	std::mt19937 gen{ rd() };

	for (auto& reel : reels)
	{
		std::shuffle(reel.begin(), reel.end(), gen);
	}

	std::uniform_int_distribution dist(1, numbers - 1); // int dis includes last number, but rotate mustn't use a middle equal to or beyond end
	int credit = 1;
	while (true)
	{
		const int left = reels[0][0];
		const int middle = reels[1][0];
		const int right = reels[2][0];
		std::cout << std::format(" {:>3} {:>3} {:>3}\n", reels[0][numbers - 1], reels[1][numbers - 1], reels[2][numbers - 1]);
		std::cout << std::format("-{:>3} {:>3} {:>3}-\n", reels[0][0], reels[1][0], reels[2][0]);
		std::cout << std::format(" {:>3} {:>3} {:>3}\n", reels[0][1], reels[1][1], reels[2][1]);
		--credit;
		int won = calculate_payout(left, middle, right);
		credit += won;
		std::cout << "won " << won << " credit = " << credit << '\n';

		std::string response;
		std::getline(std::cin, response);
		if (response == "q") // for quit
		{
			break;
		}
		for (auto& reel : reels)
		{
			std::rotate(reel.begin(), reel.begin() + dist(gen), reel.end());
		}
	}
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

	// Check closed form n * (n+1)/2
	// either with a for loop
	for (size_t i = 0; i < triangle_numbers.size(); ++i)
	{
		const int n = i + 1;
		assert(triangle_numbers[i] == n * (n + 1) / 2);
	}
	// or an algo with a mutable lambda
	assert(std::all_of(triangle_numbers.begin(), triangle_numbers.end(),
		[n = 0](int x) mutable
		{
			++n;
			return x == n * (n + 1) / 2;
		}
	));

	assert(calculate_payout(0, 1, 3) == 0);
	assert(calculate_payout(0, 0, 3) == 1);
	assert(calculate_payout(0, 3, 3) == 2);
	assert(calculate_payout(3, 0, 3) == 2);
	assert(calculate_payout(3, 3, 0) == 2);
	assert(calculate_payout(0, 0, 0) == 2);
	assert(calculate_payout(3, 3, 3) == 4);
	assert(calculate_payout(8, 8, 8) == 4);
}

int main()
{
	check_properties();
	demo_further_properties();

	triangle_machine_spins_only();
}