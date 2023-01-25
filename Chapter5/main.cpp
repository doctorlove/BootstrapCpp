#include <iostream>

#include "playing_cards.h"

using namespace cards;

int main()
{
	Card card{ 2, Suit::Club }; //after Listing 5.5

	std::cout << card << '\n';

}