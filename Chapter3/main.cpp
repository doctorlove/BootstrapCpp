#include <cassert>
#include <format>
// if you are using fmt instead, swap the last include for the next two lines and change std::format to fmt::format
//#define FMT_HEADER_ONLY
//#include <fmt/core.h>
#include <functional>
#include <iostream>
#include <optional>
#include <random>
#include <string>
#include <unordered_set>
#include <vector>


// Listing 3.1
unsigned some_const_number()
{
	return 42;
}

// Listing 3.2
unsigned input()
{
	unsigned number; //try a negative number!
	while (!(std::cin >> number))
	{
		std::cin.clear();
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		std::cout << "Please enter a number.\n>";
	}
	return number;
}

//Listing 3.3
void guess_number(unsigned number)
{
	std::cout << "Guess the number.\n>";
	unsigned guess = input();
	while (guess != number)
	{
		std::cout << guess << " is wrong. Try again\n>";
		guess = input();
	}
	std::cout << "Well done.\n";
}

// Listing 3.4 Taking optional input
// Allow a way to give up
std::optional<unsigned> read_number(std::istream& in)
{
	unsigned result{};
	if (in >> result) {
		return result;
	}
	in.clear();
	in.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	return {};
}

//Listing 3.5 Allow giving up
void guess_number_or_give_up(int number)
{
	std::cout << "Guess the number.\n>";
	std::optional<int> guess;
	while (guess = read_number(std::cin))
	{
		if (guess.value() == number)
		{
			std::cout << "Well done.\n";
			return;
		}
		std::cout << guess.value() << " is wrong. Try again\n>";
	}
	std::cout << "The number was " << number << "\n";
}


// Listing 3.9
unsigned some_random_number()
{
	std::random_device rd;
	std::mt19937 mt(rd());
	std::uniform_int_distribution<> dist(0, 100);
	return dist(mt);
}

// Listing 3.11 Function to check if a number is prime
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

// Listing 3.13, which started as listing 3.12, a function to indicate which digits are correct
// * means correct in the right place
// ^ means correct in the wrong place
// . means wrong
std::string check_which_digits_correct(unsigned number, unsigned guess)
{
	auto ns = std::format("{:0>5}", (number));
	auto gs = std::format("{:0>5}", (guess));
	std::string matches(5, '.');
	// which for guesses match... 
	for (size_t i = 0, stop = gs.length(); i < stop; ++i)
	{
		char guess_char = gs[i];
		if (i < ns.length() && guess_char == ns[i])
		{
			matches[i] = '*';
			ns[i] = '*'; // don't reuse this digit
		}
	}
	// now for guesses that don't match... 
	for (size_t i = 0, stop = gs.length(); i < stop; ++i)
	{
		char guess_char = gs[i];
		if (i < ns.length() && matches[i] != '*')
		{
			if (size_t idx = ns.find(guess_char, 0);
				idx != std::string::npos)
			{
				idx = ns.find(guess_char, idx);
				matches[i] = '^';
				ns[idx] = '^';
			}
		}
	}
	return matches;
}

void check_properties()
{
	static_assert(is_prime(2));
	static_assert(is_prime(7321));
	static_assert(is_prime(56897));
	static_assert(is_prime(41521));
	auto got = check_which_digits_correct(12347, 11779);
	assert(got == "*.^..");
	got = check_which_digits_correct(12345, 23451);
	assert(got == "^^^^^");
	got = check_which_digits_correct(12345, 12345);
	assert(got == "*****");
	got = check_which_digits_correct(48533, 12345);
	assert(got == "..^^^");
	got = check_which_digits_correct(98041, 41141);
	assert(got == "...**");
	assert(is_prime(17231));
	got = check_which_digits_correct(1723, 17231);
	assert(got == "^^^^.");
	unsigned number = 78737;
	got = check_which_digits_correct(number, 87739);
	assert(got == "^^**.");

}

// Listing 3.14 A much better number guessing game
void guess_number_with_clues(unsigned number, auto message)
{
	std::cout << "Guess the number.\n>";
	std::optional<int> guess;
	while (guess = read_number(std::cin))
	{
		if (guess.value() == number)
		{
			std::cout << "Well done.\n";
			return;
		}
		std::cout << message(number, guess.value());
		std::cout << '>';
	}
	std::cout << std::format("The number was {}\n", number);
}


// Listing 3.11 Generating a prime number
int some_prime_number()
{
	std::random_device rd;
	std::mt19937 mt{ rd() };
	std::uniform_int_distribution<int> dist{0, 99999};
	int n{};
	while (!is_prime(n))
	{
		n = dist(mt);
	}
	return n;
}

// Listing 3.18 Using all the clues
void guess_number_with_more_clues(int number, auto messages)
{
	std::cout << "Guess the number.\n>";
	std::optional<int> guess;
	while (guess = read_number(std::cin))
	{
		if (guess.value() == number)
		{
			std::cout << "Well done.";
			return;
		}
		std::cout << std::format("{:0>5} is wrong. Try again\n", guess.value());
		for (auto message : messages)
		{
			auto clue = message(guess.value());
			if (clue.length())
			{
				std::cout << clue;
				break;
			}
		}
	}
	std::cout << std::format("The number was {:0>5}\n", (number));
}


int main()
{
	check_properties();

	// guess a number without a clue
	guess_number(some_const_number());

	// alow a non-number to indicate defeat
	guess_number_or_give_up(some_const_number());

	// Guess any number with a clue:
	guess_number_with_clues(some_random_number(), [](int number, int guess) { return std::format("Your guess was too {}\n", (guess < number ? "small" : "big")); });

	// Guess a prime number:
	auto check_prime = [](int guess) {
		return std::string((is_prime(guess)) ? "" : "Not prime\n");
	};

	auto check_length = [](int guess) {
		return std::string((guess < 100000) ? "" : "Too long\n");
	};

	const int number = some_prime_number();
	auto check_digits = [number](int guess) {
		return std::format("{}\n", 
			check_which_digits_correct(number, guess));
	};
	std::vector<std::function<std::string(int)>> messages
	{
		check_length,
		check_prime,
		check_digits
	};
	guess_number_with_more_clues(number, messages);
}
