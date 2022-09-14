// guess number, then enforce five digits, then enforce prime
#include <iostream>
#include <format>
#include <random>

//lookup table of primes
//stats on the guess
#include <unordered_set>
#include <ranges>


constexpr bool is_prime(int n)
{
	//https://en.wikipedia.org/wiki/Primality_test
	if (n == 2 || n == 3)
		return true;

	if (n <= 1 || n % 2 == 0 || n % 3 == 0)
		return false;

	for (int i = 5; i * i <= n; i += 6)
	{
		if (n % i == 0 || n % (i + 2) == 0)
			return false;
	}

	return true;
}

auto generate_primes()
{
	std::unordered_set<int> primes;
	for (int i : std::views::iota(1, 99999)
		| std::views::filter(is_prime)) {
		primes.insert(i);
	}
	return primes;
}

// which_digits_correct
// needs tests; what to return? *.^ kinda?
/// 1239, 1320 -> *^^., or wot?
// maybe need to find correct, then find ones in the wrong place?
#include <string>
std::string which_digits_correct(unsigned number, unsigned guess) //TODO refactor- or stop the two for loops
{
	//auto ns = std::to_string(number);
	//auto gs = std::to_string(guess);
	// what if not same length? 
	auto ns = std::format("{:0>5}", (number));
	auto gs = std::format("{:0>5}", (guess));
	// maybe enforce before this point
//	std::string matches(std::min(ns.length(), gs.length()), '.'); // or size?... also needs min... 
	std::string matches(5, '.');
	for (size_t i = 0, stop = gs.length(); i < stop; ++i)
	{
		char guess_char = gs[i];
		if (i<ns.length() && guess_char == ns[i])
		{
			matches[i] = '*'; 
			ns[i] = '*'; // don't reuse this digit
		}
	}
	// now guesses that don't match... 
	for (size_t i = 0, stop = gs.length(); i < stop; ++i)
	{
		char guess_char = gs[i];
		if (i < ns.length() && guess_char != ns[i])
		{
			// string.find cf npos - the horror or C++23 https://en.cppreference.com/w/cpp/string/basic_string/contains
		//		if (ns.contains(gs[i])) // and not in gs before this point, unless there are two... damnit
			size_t idx = 0u; // start at 0
			if (ns.find(guess_char, idx) != std::string::npos)
			{
				idx = ns.find(guess_char, idx); //damn, did it again - initialise in the if?
				matches[i] = '^';
				ns[idx] = '^'; // don't reuse this digit
			}
		}
	}
	return matches;
}

#include <cassert>
void properties()
{
	auto got = which_digits_correct(12345, 23451);
	assert(got=="^^^^^");
	got = which_digits_correct(12345, 12345);
	assert(got == "*****");
	//48533
	got = which_digits_correct(48533, 12345);
	assert(got == "..^^^");
	got = which_digits_correct(98041, 41141);
	assert(got == "...**");
	assert(is_prime(17231));
	got = which_digits_correct(1723, 17231);
	assert(got == "^^^^.");
}


unsigned input() // how to signal stopping? - exceptions v a pair
{
	unsigned number; //try a negative number!
	while (!(std::cin >> number))
	{
		//throw std::exception(); ... aborts
		// need to clear the rest
		std::cin.clear();
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		std::cout << "Please enter a number.\n";
	}
	return number;
}

// Or from Pete Summerlad in CVu Sept 2022
#include <optional>
std::optional<unsigned> read_number(std::istream& in) // testable
{
	unsigned result{};
	if (in >> result) {
		return result;
	}
	return {}; // what if negative?
}

unsigned some_const_number()
{
	return 42;
}

unsigned some_number()
{
	std::random_device rd; // hard to test! also, only called once, but in general more likely to be called more often...
	std::mt19937 mt(rd());
	//std::uniform_int_distribution<unsigned> dist(0, 100); // closed [l, u]... see max fn
	std::uniform_int_distribution<> dist(0u, 100u); // which is clearer?
	//auto top = dist.max();
	return dist(mt);
}

template<typename fn>
void guess_any_number(unsigned number, fn message)
{
	std::cout << "Guess the number.\n";
	unsigned guess;
	while ((guess = input()) && guess != number) // we really want a way to stop, once we don't actually know the number
	{
		std::cout << guess << " is wrong. Try again\n"; // validate? ... five digits... beware negative numbers
		std::cout << message(number, guess);
	}
	std::cout << "Well done.";
}


int some_prime_number()
{
	std::random_device rd; // hard to test! also, only called once, but in general more likely to be called more often...
	std::mt19937 mt(rd());
	//std::uniform_int_distribution<unsigned> dist(0, 100); // closed [l, u]... see max fn
	std::uniform_int_distribution<> dist(0u, 99999u); // which is clearer?
	unsigned n = 1; // or int or what?
	while (!is_prime(n))
	{
		n = dist(mt);
	}
	return n;
}

int main()
{
	properties();
	// how many digits correct? etc... ... maybe send in feedback/reply fn? also if prime or not
	// guess_any_number(some_number(), [](int number, int guess) { return std::format("Your guess was too {}\n", (guess < number ? "small" : "big")); });
	auto primes = generate_primes();
	guess_any_number(some_prime_number(), [&primes](int number, int guess) {
		bool prime = primes.contains(guess);
			return std::format("{}\n{}\n",
				prime?"Prime":"Not prime", // maybe just say not prime, if not prime
				which_digits_correct(number, guess));
		}
	);
	// how to report the number is not prime? 
}