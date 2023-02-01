#include <algorithm>
#include <random>

#include "playing_cards.h"

namespace cards
{

	// Listing 5.8 Define operator << for a card, improved in Listing 5.10 
	//std::ostream& operator<<(std::ostream& os, const Card& card)
	//{
	//	os << card.value << " of " << static_cast<int>(card.suit) << '\n';
	//	return os;
	//}

	//Listing 5.9 Turn an enum value into a string
	std::string to_string(Suit suit)
	{
		switch (suit)
		{
		case Suit::Heart:
			return "Hearts";
		case Suit::Diamond:
			return "Diamonds";
		case Suit::Club:
			return "Clubs";
		case Suit::Spade:
			return "Spades";
		}
		return "?";
	}

	// Listing 5.10 Use the enum name rather than value
	std::ostream& operator<<(std::ostream& os, const Card& card)
	{
		os << card.value << " of " << to_string(card.suit);
		return os;
	}

	// Listing 5.11 Build a deck of cards, given as
	// std::array<Card, 52> create_deck() in the book
	std::array<Card, 52> create_deck_first_way()
	{
		std::array<Card, 52> deck;
		auto card = deck.begin();
		for (auto suit :
			{ Suit::Heart, Suit::Diamond, Suit::Club, Suit::Spade })
		{
			for (int value = 1; value <= 13; value++)
			{
				card->suit = suit;
				card->value = value;
				++card;
			}
		}
		return deck;
	}

	// Listing 5.14 increment our enum
	Suit& operator++(Suit& suit)
	{
		// Dangerous! https://stackoverflow.com/questions/3475152/why-cant-i-increment-a-variable-of-an-enumerated-type
		using IntType = typename std::underlying_type<Suit>::type;
		if (suit == Suit::Spade)
			suit = Suit::Heart;
		else
			suit = static_cast<Suit>(static_cast<IntType>(suit) + 1);
		return suit;
	}

	//Listing 5.15 Generating the deck of cards
	std::array<Card, 52> create_deck()
	{
		std::array<Card, 52> deck;
		int value = 1;
		Suit suit = Suit::Heart;
		std::ranges::generate(deck, [&value, &suit]() {
			if (value > 13)
			{
				value = 1;
				++suit;
			}
			return Card(value++, suit);
		});
		return deck;
	}

	// Listing 5.18 Shuffle the cards
	void shuffle_deck(std::array<Card, 52>& deck)
	{
		std::random_device rd;
		std::mt19937 gen{ rd() };
		std::ranges::shuffle(deck, gen);
	}

	// Listing 5.19 Is the guess correct?
	bool guess_correct(char guess, const Card & current, const Card & next)
	{
		return (guess == 'h' && next > current) 
			    || (guess == 'l' && next < current);
	}

	//Listing 5.20 Higher lower game
	void higher_lower()
	{
		auto deck = create_deck();
		shuffle_deck(deck);

		size_t i = 0;
		while (i + 1 < deck.size())
		{
			std::cout << deck[i] << ": Next card higher (h) or lower (l)?\n>";
			char c;
			std::cin >> c;
			bool ok = guess_correct(c, deck[i], deck[i + 1]);
			if (!ok)
			{
				std::cout << "Next card was " << deck[i + 1] << '\n';
				break;
			}
			++i;
		}
		std::cout << "You got " << i << " guess correct\n";
	}
}
