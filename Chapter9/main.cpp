#include <algorithm>
#include <cassert>
#include <concepts>
#include <execution>
#include <format>
#include <iostream>
#include <iterator>
#include <map>
#include <numeric>
#include <optional>
#include <random>
#include <ranges>
#include <set>
#include <string>
#include <utility>
#include <variant>
#include <vector>

using Reel = std::vector<int>;

// Listing 9.1 Make the first few triangle numbers
// constexpr added in section 9.2
constexpr std::vector<int> make_triangle_numbers(int count)
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

// Listing 9.5 Setup reels
template<std::invocable<std::vector<Reel>::iterator, std::vector<Reel>::iterator> T>
constexpr std::vector<Reel> make_reels(int numbers, int number_of_reels, T shuffle)
{
	std::vector<Reel> reels(number_of_reels, make_triangle_numbers(numbers));

	for (auto& reel : reels)
	{
		shuffle(reel.begin(), reel.end());
	}
	return reels;
}

// Listing 9.6 Display reels
void show_reels(std::ostream& s, const std::vector<int>& left, const std::vector<int>& middle, const std::vector<int>& right)
{
	s << std::format(" {:>3} {:>3} {:>3}\n", left.back(), middle.back(), right.back());
	s << std::format("-{:>3} {:>3} {:>3}-\n", left[0], middle[0], right[0]);
	s << std::format(" {:>3} {:>3} {:>3}\n", left[1], middle[1], right[1]);
}

// Listing 9.7 Calculate payout
int calculate_payout_v1(int left, int middle, int right)
{
	int payout = 0;
	if (left == middle && middle == right)
	{
		payout = 2;
	}
	else if (left == middle || middle == right || left == right)
	{
		payout = 1;
	}
	return payout;
}

// Listing 9.9 Finding frequencies using a parameter pack
// We considered
//template<typename... Ts>
//std::map<int, size_t> frequencies(Ts... numbers)
// and then
//std::map<int, size_t> frequencies(auto ...numbers) 
// but settled on
std::map<int, size_t> frequencies(std::convertible_to<int> auto... numbers)
{
	std::map<int, size_t> counter{};
	for (int i : { static_cast<int>(numbers)... })
		counter[i]++;
	return counter;
}

// Listing 9.11 A fairer payout
int calculate_payout(int left, int middle, int right)
{
	std::map<int, size_t> counter = frequencies(left, middle, right);
	auto it = std::max_element(counter.begin(), counter.end(),
		[](auto it1, auto it2) { return it1.second < it2.second; });
	if (it != counter.end())
	{
		int digit = it->first;
		size_t count = it->second;
		constexpr int value[] = { 0, 0, 50, 500 }; // TODO decide weightings
		auto pay = ((digit == 8 || digit == 3) ? 2 : 1) * value[count];
		return pay;
	}
	return 0;
}

// Listing 9.8 A simple one armed bandit game
void triangle_machine_spins_only()
{
	constexpr int numbers = 20;
	constexpr size_t number_of_reels = 3u;
	std::random_device rd;
	std::mt19937 gen{ rd() };
	auto shuffle = [&gen](auto begin, auto end) { std::shuffle(begin, end, gen); };
	std::vector<Reel> reels = make_reels(numbers, number_of_reels, shuffle);

	std::uniform_int_distribution dist(1, numbers - 1);
	int credit = 1;
	while (true)
	{
		show_reels(std::cout, reels[0], reels[1], reels[2]);
		const int won = calculate_payout_v1(reels[0][0]%10, reels[1][0]%10, reels[2][0]%10);
		--credit;
		credit += won;
		std::cout << "won " << won << " credit = " << credit << '\n';

		std::string response;
		std::getline(std::cin, response);
		if (response != "")
		{
			break;
		}
		for (auto& reel : reels)
		{
			std::rotate(reel.begin(), reel.begin() + dist(gen), reel.end());
		}
	}
}

// Listing 9.12 Allow more options
struct Hold {};
struct Nudge {};
struct Spin {};
using options = std::variant<Hold, Nudge, Spin>;

// Listing 9.13 Map a character to an action
constexpr std::optional<options> get_option(char c)
{
	switch (c)
	{
	case 'h':
		return Hold{};
		break;
	case 'n':
		return Nudge{};
		break;
	case 's':
		return Spin{};
		break;
	}
	return {};
}

// Listing 9.14 Check for holds, nudges or spins
constexpr std::optional<std::vector<options>> parse_input(const std::string & response)
{
	std::vector<options> choice;
	for (char c : response)
	{
		auto first = get_option(c);
		if (first)
		{
			choice.push_back(first.value());
		}
		else
		{
			return {};
		}
	}
	return choice.empty() ? std::vector<options>{Spin{}, Spin{}, Spin{}} : choice;
}

// Listing 9.15 Check for Enter pressed
constexpr std::optional<std::vector<options>> parse_enter(const std::string& response)
{
	if (response.empty())
	{
		return std::vector<options>{Spin{}, Spin{}, Spin{}};
	}
	else
	{
		return {};
	}
}

// Listing 9.17 Bring operator() into scope in a class 
// called Overload in the text
template <typename T>
struct Overload1 : T {
	using T::operator();
};

// Listing 9.18 The Overload pattern
template <typename... Ts>
struct Overload : Ts... {
	using Ts::operator()...;
};

// Listing 9.20 Move the reels
template<typename T>
void move_reel(std::vector<int>& reel, options opt, T random_fn)
{
	auto RollMethod = Overload{
		[](Hold) {},
		[&reel](Nudge) { std::rotate(reel.begin(), reel.begin() + 1, reel.end()); },
		[&reel, &random_fn](Spin) { std::rotate(reel.begin(), reel.begin() + random_fn(), reel.end()); },
	};

	std::visit(RollMethod, opt);
}

// Listing 9.21 An improved triangle number machine
void triangle_machine()
{
	constexpr int numbers = 20;
	constexpr size_t number_of_reels = 3u;
	std::random_device rd;
	std::mt19937 gen{ rd() };
	auto shuffle = [&gen](auto begin, auto end) { std::shuffle(begin, end, gen); };
	std::vector<Reel> reels = make_reels(numbers, number_of_reels, shuffle);

	std::uniform_int_distribution dist(1, numbers - 1);
	auto random_fn = [&gen, &dist]() { return dist(gen); };
	int credit = 1;
	while (true)
	{
		show_reels(std::cout, reels[0], reels[1], reels[2]);
		const int won = calculate_payout(reels[0][0] % 10, reels[1][0] % 10, reels[2][0] % 10);
		--credit;
		credit += won;
		std::cout << "won " << won << " credit = " << credit << '\n';

		std::string response;
		std::getline(std::cin, response);
		std::optional<std::vector<options>> choice = won ? parse_enter(response) : parse_input(response);
		if (!choice)
		{
			break;
		}

		for (auto [reel, option] : std::views::zip(reels, choice.value()))
		{
			move_reel(reel, option, random_fn);
		}
	}
}

// Listing 9.10 Fold example
// TODO but how to deal with 0 param? See https://www.fluentcpp.com/2021/03/12/cpp-fold-expressions/
template <typename H, typename ... Ts>
auto add(H head, Ts... tail)
{
	return (head + ... + tail);
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

	assert(calculate_payout_v1(0, 1, 3) == 0);
	assert(calculate_payout_v1(0, 0, 3) == 1);
	assert(calculate_payout_v1(0, 3, 3) == 1);
	assert(calculate_payout_v1(3, 0, 3) == 1);
	assert(calculate_payout_v1(3, 3, 0) == 1);
	assert(calculate_payout_v1(0, 0, 0) == 2);
	assert(calculate_payout_v1(3, 3, 3) == 2);
	assert(calculate_payout_v1(8, 8, 8) == 2);

	assert(calculate_payout(0, 1, 3) == 0);
	assert(calculate_payout(0, 0, 3) == 50);
	assert(calculate_payout(0, 3, 3) == 100);
	assert(calculate_payout(3, 0, 3) == 100);
	assert(calculate_payout(3, 3, 0) == 100);
	assert(calculate_payout(0, 0, 0) == 500);
	assert(calculate_payout(3, 3, 3) == 1000);
	assert(calculate_payout(8, 8, 8) == 1000);

	constexpr auto no_op = [](auto begin, auto end) { };
	static_assert(make_reels(1, 1, no_op).size() == 1);

	std::vector v{ 1,2,3,4,5 };
	std::rotate(v.begin(), v.begin() + 3, v.end());
	assert(v[0] == 4);

	static_assert( parse_enter("").has_value());
	static_assert(!parse_enter("q").has_value());
	static_assert( parse_input("hhh").has_value());
	static_assert(!parse_enter("q").has_value());

	assert(1 == add(1));
	assert(6 == add(1, 2, 3));
	using namespace std::literals;
	assert(add("Hello"s, "again"s, "world"s)=="Helloagainworld"s);

	auto overload = Overload1{ []() { return 0; } };
	assert(overload() == 0);
}

int main()
{
	check_properties();
	demo_further_properties();

	triangle_machine_spins_only();

	std::cout << "Hold (h), spin(s), nudge(n) or Enter for spins\n";
	triangle_machine();
}