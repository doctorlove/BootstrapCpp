#include <iostream>
#include <format>
#include <random>

#include <unordered_set>
#include <ranges>
#include <string>


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

// Which digits are correct
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
		if (i<ns.length() && guess_char == ns[i])
		{
			matches[i] = '*'; 
			ns[i] = '*'; // don't reuse this digit
		}
	}
	// now for guesses that don't match... 
	for (size_t i = 0, stop = gs.length(); i < stop; ++i)
	{
		char guess_char = gs[i];
		if (i < ns.length() && guess_char != ns[i] && matches[i] != '^' && matches[i] != '*')
		{
			// string.find cf npos - the horror or C++23 https://en.cppreference.com/w/cpp/string/basic_string/contains
		//		if (ns.contains(gs[i]))
			size_t idx = 0u;
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
void check_properties()
{
	auto got = check_which_digits_correct(12347, 11779);
	assert(got == "*.^..");
	got = check_which_digits_correct(12345, 23451);
	assert(got=="^^^^^");
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


unsigned input() // how to signal stopping? - exceptions v a pair
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

// Allow a way to give up
#include <optional>
std::optional<unsigned> read_number(std::istream& in)
{
	unsigned result{};
	if (in >> result) {
		return result;
	}
	return {}; // what if negative? or too big
}
//also getline: https://stackoverflow.com/questions/7868936/read-file-line-by-line-using-ifstream-in-c

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
	return dist(mt);
}

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

void guess_number_or_give_up(int number)
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
		std::cout << std::format("{} is wrong. Try again\n>",
								guess.value());
	}
	std::cout << std::format("The number was {}\n", number);
}


template<typename fn>
void guess_number_with_clues(unsigned number, fn message)
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
		std::cout << message(number, guess.value());
		std::cout << '>';
	}
	std::cout << std::format("The number was {}\n", number);
}

template<typename fn>
void guess_any_number_or_give_up(unsigned number, fn message)
{
	std::cout << "Guess the number.\n";
	std::optional<unsigned> guess;
	while (guess = read_number(std::cin)) // drops out of loop if no value
	{
		if (guess.value() == number)
		{
			std::cout << "Well done.";
			return;
		}
		std::cout << guess.value() << " is wrong. Try again\n>";
		std::cout << message(number, guess.value()); 
	}
	std::cout << std::format("The number was {:0>5}\n", (number));
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
	check_properties();

	// guess a number without a clue
	//guess_number(some_const_number());

	//guess_number_or_give_up(some_const_number());

	// Guess any number with a clue:
	guess_number_with_clues(some_const_number(), [](int number, int guess) { return std::format("Your guess was too {}\n", (guess < number ? "small" : "big")); });
	return 0;
	// Guess a prime number:
	auto primes = generate_primes();
	guess_any_number_or_give_up(some_prime_number(), [&primes](int number, int guess) {
			bool prime = primes.contains(guess);
			return std::format("{}\n{}\n",
				prime?"Prime":"Not prime", // maybe just say not prime, if not prime... also test length of number
				check_which_digits_correct(number, guess));
		}
	);
}