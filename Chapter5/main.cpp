#include <iostream>

#include "playing_cards.h"

#include <cassert>
#include <set>
void check_properties()
{
	using namespace cards;
	auto cards = create_deck();
	assert(cards.size() == 52);
	auto as_set = std::set<Card>(cards.begin(), cards.end());//need comparator
	assert(as_set.size() == 52);

	assert(is_guess_correct('l', { 9, Suit::Club }, { 6, Suit::Club }));
	assert(is_guess_correct('h', { 5, Suit::Club }, { 7, Suit::Spade }));
	assert(is_guess_correct('h', { 3, Suit::Spade }, { 5, Suit::Club }));

	assert(is_guess_correct('l', 
			std::variant<Card, Joker>(Card{ 9, Suit::Club }),
			std::variant<Card, Joker>(Card{ 6, Suit::Club })
		)
	);
	assert(is_guess_correct('l',
			std::variant<Card, Joker>(Joker{}),
			std::variant<Card, Joker>(Card{ 6, Suit::Club })
		)
	);
	assert(is_guess_correct('l',
		std::variant<Card, Joker>(Card{ 6, Suit::Club }),
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
	cards::Card card{ 2, cards::Suit::Club }; //after Listing 5.5
	std::cout << card << '\n';

	// This is UB if we don't initialize the fields
	cards::Card dangerous_card;
	std::cout << dangerous_card <<'\n';

	std::cout << cards::Card{ 1, cards::Suit::Heart } << '\n';

	check_properties();

	std::cout << "Higher/lower game - aces low\n";
	cards::higher_lower();

	std::cout << "Higher/lower game - aces low, Jokers a free go\n";
	cards::higher_lower_with_jokers();
}