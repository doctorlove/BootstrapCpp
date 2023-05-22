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

// Listing 7.12 A fake generator for testing // TODO did just return 0
struct FakeGen
{
	int x = 0;
	using result_type = unsigned;
	result_type operator()() {
		return x; 
	}
	static constexpr result_type min() { return 0; }
	static constexpr result_type max() { return 10; } // TODO hmmm maybe template too?
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

	auto select_first = [](auto lb, auto ub, auto dest) {
		*dest = *lb;
	};
	auto [word1, definition1, offset1_2] = select_overlapping_word_from_dictionary("sprint", second_2, select_first);
	assert(word1 == "integer");
	auto [word2, definition2, offset2_2] = select_overlapping_word_from_dictionary("minus", second_2, select_first);
	assert(word2 == "struct");
	auto [word3, definition3, offset3_2] = select_overlapping_word_from_dictionary("vector", first_2, select_first);
	assert(word3 == "torch");

	auto [word4, definition4, offset4_2] = select_overlapping_word_from_dictionary("class", first_2, select_first);
	assert(word4 == "assault");
	assert(offset4 == 2);

	auto [no_word, no_definition, no_offset] = select_overlapping_word_from_dictionary("class", {}, select_first);
	assert(no_word == "");
	assert(no_offset == -1);

	std::string word("aaa");
	auto [g1, d1, o1] = select_overlapping_word_from_dictionary(word,
		{ {"a", "1"}, {"aaa", "1"}, {"aab", "1"}, {"aabb", "2"}, {"aabc", "2"}, {"abbc", "3"} },
		select_first);
	assert(g1 == "aaa");
	auto [g2, d2, o2] = select_overlapping_word_from_dictionary(word,
		{ {"a", "1"}, {"aaaa", "1"}, {"aaab", "1"}, {"aaabb", "2"}, {"aaabc", "2"}, {"aabbc", "3"} },
		select_first);
	assert(g2 == "aaaa");

	auto select_last = [](auto lb, auto ub, auto dest) {
		*dest = *(--ub);
	};
	auto [g1b, d1b, o1b] = select_overlapping_word_from_dictionary(word,
		{ {"a", "1"}, {"aaa", "1"}, {"aab", "1"}, {"aabb", "2"}, {"aabc", "2"}, {"abbc", "3"} },
		select_last);
	assert(g1b == "aabc");
	auto [g2b, d2b, o2b] = select_overlapping_word_from_dictionary(word,
		{ {"a", "1"}, {"aaaa", "1"}, {"aaab", "1"}, {"aaabb", "2"}, {"aaabc", "2"}, {"aabbc", "3"} },
		select_last);
	assert(g2b == "aabbc");
}

// Listing 7.1 Creating and displaying a map, along with some one liners considered in the text
void warm_up()
{
	std::map<std::string, std::string> dictionary;
	dictionary["assume"] = "take for granted, take to be the case";
	std::string new_value = dictionary["fictional"]; // inserts "" against fictional
	for (const auto & item : dictionary)
	{
		std::cout << item.first << " : " << item.second << '\n';
	}
	// Extra ideas considered in the text
	dictionary["fictional"] = "made up"; // replaces ""
	// whereas insert tells us if something already exists
	auto result = dictionary.insert({ "insert", "place inside" });
	std::cout << std::boolalpha << result.first->first << " added? " << result.second << " definition: " << result.first->second << '\n';
	auto next_result = dictionary.insert({ "fictional", "not factual" });
	std::cout << std::boolalpha << next_result.first->first << " added? " << next_result.second << " definition: " << next_result.first->second << '\n';
}

// Listing 7.2 Using structure bindings to access map items
void structure_bindings()
{
	std::map<std::string, std::string> dictionary;
	dictionary["assume"] = "take for granted, take to be the case";
	std::string new_word = dictionary["fictional"];
	for (const auto & [key, value] : dictionary)
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

// Listing 7.7 Play the first version of answer smash
void hard_coded_game()
{
	const std::map<std::string, std::string> keywords{
		{"char", "type for character representation which can be most"
				"efficiently processed on the target system"},
		{"class", "user defined type with private members by default"},
		{"struct", "user defined type with public members by default"},
		{"vector", "sequential container supporting dynamic resizing"},
		{"template", "family of classes or functions parameterized by one or more parameters"},
	};

	const std::map<std::string, std::string> dictionary{
		{"assume", "take for granted, take to be the case"},
		{"harsh", "coarse, large - grained or rough to the touch"},
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

	std::cout << "Hard coded game\n\n";
	hard_coded_game();

	std::cout << "Full game\n\n";
	full_game();
}