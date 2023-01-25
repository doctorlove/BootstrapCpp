#pragma once

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
		int value;
		Suit suit;
		// And added in a later listing
		//friend auto operator<=>(const Card&, const Card&) = default; //means aces low
	};

	std::ostream& operator<<(std::ostream& os, const Card& card);
}
