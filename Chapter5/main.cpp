#include <iostream>

#include "playing_cards.h"

#include <cassert>
#include <set>
void check_properties()
{
	using namespace cards;

	try
	{
		FaceValue value(-1);
		assert(false);
	}
	catch (const std::exception & )
	{
		// could assert more here, but it shows an exception was thrown
	}

	FaceValue value(1);
	assert(value.value() == 1);

	auto cards = create_deck();
	assert(cards.size() == 52);
	auto as_set = std::set<Card>(cards.begin(), cards.end());//need comparator
	assert(as_set.size() == 52);

	assert(is_guess_correct('l', { FaceValue(9), Suit::Clubs }, { FaceValue(6), Suit::Clubs }));
	assert(is_guess_correct('h', { FaceValue(5), Suit::Clubs }, { FaceValue(7), Suit::Spades }));
	assert(is_guess_correct('h', { FaceValue(3), Suit::Spades }, { FaceValue(5), Suit::Clubs }));

	assert(is_guess_correct('l', 
			std::variant<Card, Joker>(Card{ FaceValue(9), Suit::Clubs }),
			std::variant<Card, Joker>(Card{ FaceValue(6), Suit::Clubs })
		)
	);
	assert(is_guess_correct('l',
			std::variant<Card, Joker>(Joker{}),
			std::variant<Card, Joker>(Card{ FaceValue(6), Suit::Clubs })
		)
	);
	assert(is_guess_correct('l',
		std::variant<Card, Joker>(Card{ FaceValue(6), Suit::Clubs }),
		std::variant<Card, Joker>(Joker{})
		)
	);
	assert(is_guess_correct('l',
		std::variant<Card, Joker>(Joker{}),
		std::variant<Card, Joker>(Joker{})
		)
	);
}

int main()
{
	std::cout << "Some warm up\n";
	cards::CardVersion1 card{ 2, cards::Suit::Clubs }; //after Listing 5.5
	std::cout << card << '\n';

	// This is UB if we don't initialize the fields
	cards::Card dangerous_card;
	std::cout << dangerous_card <<'\n';

	std::cout << cards::Card{ cards::FaceValue(1), cards::Suit::Hearts } << '\n';

	check_properties();

	std::cout << "Higher/lower game - aces low\n";
	cards::higher_lower();

	std::cout << "Higher/lower game - aces low, Jokers a free go\n";
	cards::higher_lower_with_jokers();
}