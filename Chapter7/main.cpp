#include <algorithm>
#include <cassert>
#include <cctype>
#include <iostream>
#include <string>

#include "Smash.h"


// Listing 7.5 Find an overlapping word
std::pair<std::string, int> find_overlapping_word_v1(std::string word,
	const std::map<std::string, std::string>& dictionary)
{
	size_t offset = 1;
	while (offset < word.size())
	{
		auto stem = word.substr(offset);
		for (const auto & [k, v] : dictionary)
		{
			auto key_stem = k.substr(0, stem.size()); // v1, for v2 use lower bound
			if (key_stem == stem)
			{
				return std::make_pair(k, offset);
			}
		}
		++offset;
	}
	return std::make_pair("", -1);
}

// Listing 7.12 A fake generator for testing
struct FakeGen
{
	using result_type = unsigned;
	result_type operator()() { return 0; }
	static constexpr result_type min() { return 0; }
	static constexpr result_type max() { return 1; }
};

void check_properties()
{
	using namespace smashing;
	std::map<std::string, std::string> first{
		{ "sprint", ""},
		{ "swim", ""},
		{ "torch", ""},
		{ "assault", ""}
	};
	std::map<std::string, std::string> second{
		{ "integer", ""},
		{ "immuatble", ""},
		{ "class", ""},
		{ "struct", ""},
		{ "vector", ""},
	};

	auto [got1, offset1] = find_overlapping_word("sprint", second);
	assert(got1 == "integer");
	auto [got2, offset2] = find_overlapping_word("minus", second);
	assert(got2 == "struct");
	auto [got3, offset3] = find_overlapping_word("vector", first);
	assert(got3 == "torch");

	auto [got4, offset4] = find_overlapping_word("class", first);
	assert(got4 == "assault");
	assert(offset4 == 2);

	auto [got, offset] = find_overlapping_word("class", {});
	assert(got == "");
	assert(offset == -1);

	std::multimap<std::string, std::string> first_2{
		{ "sprint", ""},
		{ "swim", ""},
		{ "torch", ""},
		{ "assault", ""}
	};
	std::multimap<std::string, std::string> second_2{
		{ "integer", ""},
		{ "immuatble", ""},
		{ "class", ""},
		{ "struct", ""},
		{ "vector", ""},
	};

	auto [word1, definition1, offset1_2] = select_overlapping_word_from_dictionary<FakeGen>("sprint", second_2, FakeGen());
	assert(word1 == "integer");
	auto [word2, definition2, offset2_2] = select_overlapping_word_from_dictionary("minus", second_2, FakeGen());
	assert(word2 == "struct");
	auto [word3, definition3, offset3_2] = select_overlapping_word_from_dictionary("vector", first_2, FakeGen());
	assert(word3 == "torch");

	auto [word4, definition4, offset4_2] = select_overlapping_word_from_dictionary("class", first_2, FakeGen());
	assert(word4 == "assault");
	assert(offset4 == 2);

	auto [no_word, no_definition, no_offset] = select_overlapping_word_from_dictionary("class", {}, FakeGen());
	assert(no_word == "");
	assert(no_offset == -1);
}

// Listing 7.1 Creating and displaying a map, along with some one liners considered in the text
void warm_up()
{
	std::map<std::string, std::string> dictionary;
	dictionary["assault"] = "physical attack";
	std::string new_word = dictionary["fictional"]; // inserts "" against fictional
	for (auto item : dictionary)
	{
		std::cout << item.first << " : " << item.second << '\n';
	}
	dictionary["fictional"] = "made up"; // replaces ""
	auto result = dictionary.insert({ "insert", "place inside" });
	std::cout << std::boolalpha << result.first->first << " added? " << result.second << " definition: " << result.first->second << '\n';
	auto next_result = dictionary.insert({ "fictional", "not factual" });
	std::cout << std::boolalpha << next_result.first->first << " added? " << next_result.second << " definition: " << next_result.first->second << '\n';
}

// Listing 7.2 Using structure bindings to access map items
void structure_bindings()
{
	std::map<std::string, std::string> dictionary;
	dictionary["assault"] = "physical attack";
	std::string new_word = dictionary["fictional"];
	for (auto [key, value] : dictionary)
	{
		std::cout << key << " : " << value << '\n';
	}
	dictionary["fictional"] = "made up"; // replaces ""
	auto [it, result] = dictionary.insert({ "insert", "place inside" });
	std::cout << std::boolalpha << it->first << " added? " << result << " definition: " << it->second << '\n';
	auto [next_it, next_result] = dictionary.insert({ "fictional", "not factual" });
	std::cout << std::boolalpha << next_it->first << " added? " << next_result << " definition: " << it->second << '\n';
}

// Listing 7.15, Proper answer smash game but shown directly in main in the text
void full_game()
{
	using namespace smashing;
	const auto dictionary = load_dictionary(R"(dictionary.csv)");
	const auto keywords = load_dictionary(R"(keywords.csv)");
	answer_smash(keywords, dictionary);
}

void hard_coded_game()
{
	// Listing 7.3 Using operator [] to populate a map
	std::map<std::string, std::string> keywords;
	keywords["char"] = "type for character representation which can be most efficiently processed on the target system";
	keywords["class"] = "user defined type with private members by default";
	keywords["struct"] = "user defined type with public members by default";
	keywords["vector"] = "sequential container supporting dynamic resizing";
	keywords["template"] = "used for generic code";

	// Listing 7.4 Using initializer lists to populate a map
	const std::map<std::string, std::string> dictionary{
		{"assault", "physical attack" },
		{"harsh", "coarse, large - grained or rough to the touch" },
		{"table", "piece of furniture"},
		{"tease", "mock, make fun of"},
		{"torch", "lit stick carried in one's hand"},
	};
	smashing::simple_answer_smash(keywords, dictionary);
}

int main()
{
	check_properties();

	warm_up();
	std::cout << "\n\n";

	hard_coded_game();

	full_game();
}