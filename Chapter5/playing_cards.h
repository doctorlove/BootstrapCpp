#pragma once

#include <array>
#include <compare>
#include <iostream>
#include <stdexcept>
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

	//Listing 5.5 and 5.8, just called Card in the text
	struct CardVersion1
	{
		int value{};
		Suit suit{};
	};

	std::ostream& operator<<(std::ostream& os, const CardVersion1& card);

	//Listing 5.7 A type for a face value
	class FaceValue
	{
	public:
		explicit FaceValue(int value) : value_(value) 
		{
			if (value_ < 1 || value_ > 13)
			{
				throw std::invalid_argument("Face value invalid");
			}
		}
		int value() const
		{
			return value_;
		}
		auto operator<=>(const FaceValue&) const = default;//Added in Listing 5.21 to provide less than
	private:
		int value_;
	};

	//Listing 5.10 A card class
	class Card
	{
	public:
		Card() = default;
		Card(FaceValue value, Suit suit) :
			value_(value),
			suit_(suit)
		{
		}
		FaceValue value() const { return value_; }
		Suit suit() const { return suit_; }

		//Less than considered in listing, 5.20
		//bool operator<(const Card&) const
		//{
		//	return value < other.value&& suit < other.suit;
		//}

		// Added in Listing 5.21 to provide less than, and more besides
		auto operator<=>(const Card&) const = default;//means aces low
	private:
		FaceValue value_{1};
		Suit suit_{};
	};


	std::ostream& operator<<(std::ostream& os, const Card& card);  // Listing 5.10

	// various other functions, defined in the source file and declared here
	Suit& operator++(Suit& suit);
	std::string to_string(Suit suit);
	std::string to_string(FaceValue value);

	std::array<Card, 52> create_deck();
	void shuffle_deck(std::array<Card, 52>& deck);

	bool is_guess_correct(char guess, const Card& current, const Card& next);
	void higher_lower();

	//Listing 5.26 A joker
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
