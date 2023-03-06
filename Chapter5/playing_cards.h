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
		Hearts,
		Diamonds,
		Clubs,
		Spades
	};

	//Listing 5.5 and 5.7, just called Card in the text
	struct CardVersion1
	{
		int value{};
		Suit suit{};

		//Less than considered in listing, 5.16
		//bool operator<(const Card&) const
		//{
		//	return value < other.value&& suit < other.suit;
		//}
		 
	};

	//Listing 5.9 A card class
	class Card
	{
	public:
		Card() = default;
		Card(int value, Suit suit) :
			value_(value),
			suit_(suit)
		{
		}
		int value() const { return value_; }
		Suit suit() const { return suit_; }

		// Added in Listing 5.20 to provide less than, and more besides
		auto operator<=>(const Card&) const = default;//means aces low
	private:
		int value_{};
		Suit suit_{};
	};


	std::ostream& operator<<(std::ostream& os, const Card& card);  // Listing 5.10

	// various other functions, defined in the source file and declared here
	Suit& operator++(Suit& suit);
	std::string to_string(Suit suit);
	std::string card_value_to_string(int value);

	std::array<Card, 52> create_deck();
	void shuffle_deck(std::array<Card, 52>& deck);

	bool is_guess_correct(char guess, const Card& current, const Card& next);
	void higher_lower();

	//Listing 5.25 A joker
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
