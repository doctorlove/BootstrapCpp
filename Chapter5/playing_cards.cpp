#include "playing_cards.h"

namespace cards
{

	// Listing 5.8
	std::ostream& operator<<(std::ostream& os, const Card& card)
	{
		os << card.value << " of " << static_cast<int>(card.suit) << '\n';
		return os;
	}

}
