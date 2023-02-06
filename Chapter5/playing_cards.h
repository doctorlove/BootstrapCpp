#pragma once

#include <array>
#include <compare>
#include <iostream>
#include <string>
#include <variant>

namespace cards
{
	//Listing 5.4
	enum class Suit {
		Heart,
		Diamond,
		Club,
		Spade
	};

	//Listing 5.5 and 5.7
	struct Card
	{
		int value{};
		Suit suit{};

		//Less than considered in listing, 5.16
		//bool operator<(const Card&) const
		//{
		//	return value < other.value&& suit < other.suit;
		//}
		 
		// Added in Listing 5.19
		auto operator<=>(const Card&) const = default;//means aces low
	};

	//Listing 5.8 A card class
	class Cardv2
	{
	public:
		Cardv2() = default;
		Cardv2(int value, Suit suit) :
			value(value),
			suit(suit)
		{
		}
		int get_value() const { return value; }
		Suit get_suit() const { return suit; }
	private:
		int value;
		Suit suit;
	};


	std::ostream& operator<<(std::ostream& os, const Card& card);  // Listing 5.9

	// avrious other functions in listing, defined in the source file and decalred here
	Suit& operator++(Suit& suit);
	std::string to_string(Suit suit);
	std::string card_value_to_string(int value);

	std::array<Card, 52> create_deck();
	void shuffle_deck(std::array<Card, 52>& deck);

	bool is_guess_correct(char guess, const Card& current, const Card& next);
	void higher_lower();

	//Listing 5.24 A joker
	struct Joker
	{
	};

	std::array<std::variant<Card, Joker>, 54> create_extended_deck();
	void shuffle_deck(std::array<std::variant<Card, Joker>, 54>& deck);
	bool is_guess_correct(char c,
		const std::variant<Card, Joker>& current,
		const std::variant<Card, Joker>& next);
	std::ostream& operator<<(std::ostream& os, const std::variant<Card, Joker>& card);
	void higher_lower_with_jokers();
}
