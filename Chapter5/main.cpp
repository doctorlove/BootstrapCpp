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

	assert(guess_correct('l', { 9, Suit::Club }, { 6, Suit::Club }));
	assert(guess_correct('h', { 5, Suit::Club }, { 7, Suit::Spade }));
	assert(guess_correct('h', { 3, Suit::Spade }, { 5, Suit::Club }));
}

int main()
{
	cards::Card card{ 2, cards::Suit::Club }; //after Listing 5.5
	std::cout << card << '\n';

	check_properties();

	cards::higher_lower();

}