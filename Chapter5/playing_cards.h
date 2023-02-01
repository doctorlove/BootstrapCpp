#pragma once

#include <array>
#include <compare>
#include <iostream>

namespace cards
{
	//Listing 5.4
	enum class Suit {
		Heart,
		Diamond,
		Club,
		Spade
	};

	//Listing 5.5
	struct Card
	{
		int value; // TODO initialize
		Suit suit;

		//Less than considered in listing, 5.16
		//bool operator<(const Card&) const
		//{
		//	return value < other.value&& suit < other.suit;
		//}
		 
		// Improved in Listing 5.17
		auto operator<=>(const Card&) const = default;//means aces low
	};

	std::ostream& operator<<(std::ostream& os, const Card& card);
	Suit& operator++(Suit& suit);
	std::string to_string(Suit suit);

	std::array<Card, 52> create_deck();
	void shuffle_deck(std::array<Card, 52>& deck);

	bool guess_correct(char guess, const Card& current, const Card& next);
	void higher_lower();
}
