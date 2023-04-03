#include <algorithm>
#include <cassert>
#include <cctype>
#include <fstream>
#include <iostream>
#include <map>
#include <string>

// Listing 7.9 Transform a string to lower case
// Based on https://en.cppreference.com/w/cpp/string/byte/tolower
std::string str_tolower(std::string s) {
	std::transform(s.begin(), s.end(), s.begin(),
		[](unsigned char c) { return std::tolower(c); }
	);
	return s;
}

// Listing 7.10 Load a file into a multimap
std::multimap<std::string, std::string> load_dictionary(const std::string& filename)
{
	std::multimap<std::string, std::string> dictionary;
	std::ifstream infile{ filename };
	if (infile)
	{
		std::string line;
		while (std::getline(infile, line))
		{
			size_t position = line.find(',');
			if (position != std::string::npos)
			{
				std::string key{ line.substr(0, position) };
				std::string value{ line.substr(position + 1) };
				key = str_tolower(key);
				//dictionary.insert({ key, value }); or
				dictionary.emplace(key, value);
			}
		}
	}
	else
	{
		// report error
		std::cout << "Failed to open " << filename << '\n';
	}
	return dictionary;
}

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

// Listing 7.8 Find an overlapping word more efficiently
std::pair<std::string, int> find_overlapping_word(std::string word,
	const std::map<std::string, std::string>& dictionary)
{
	size_t offset = 1;
	while (offset < word.size())
	{
		auto stem = word.substr(offset);
		auto [lb, ub] = dictionary.equal_range(stem);
		if (lb != dictionary.end() &&
			stem == lb->first.substr(0, stem.size()))
		{
			return std::make_pair(lb->first, offset);
		}
		++offset;
	}
	return std::make_pair("", -1);
}

#include <concepts>
#include <random>
template <std::invocable<> T>
std::pair<std::string, int> find_overlapping_word_from_dictionary(std::string word,
	const std::multimap<std::string, std::string>& dictionary, T gen)
{
	size_t offset = 1;
	while (offset < word.size())
	{
		auto stem = word.substr(offset);
		auto [lb, ub] = dictionary.equal_range(stem);
		if (lb != dictionary.end() &&
			stem == lb->first.substr(0, stem.size()))
		{
			std::vector<std::pair<std::string, std::string>> dest(1); // TODO something better?
			std::sample(lb, ub, dest.begin(), 1, gen);
			auto found = dest[0].first;
			return std::make_pair(found, offset);
		}
		++offset;
	}
	return std::make_pair("", -1);
}


void check_properties()
{
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

// Listing 7.6 Simple answer smash game
void simple_answer_smash(
	const std::map<std::string, std::string> &keywords,
	const std::map<std::string, std::string> &dictionary)
{
	for (const auto & [word, definition] : keywords)
	{
		auto [second_word, offset] = find_overlapping_word(word, dictionary);
		std::string second_definition = dictionary.at(second_word);
		std::cout << definition << "\nAND\n" << second_definition << '\n';

		std::string answer = word.substr(0, offset) + second_word;

		std::string response;
		std::getline(std::cin, response);
		if (response == answer)
		{
			std::cout << "CORRECT!!!!!!!!!\n";
		}
		else
		{
			std::cout << answer << '\n';
		}
		std::cout << word << ' ' << second_word << "\n\n\n";
	}
}

// Listing 7.? Better answer smash game
void answer_smash(
	const std::multimap<std::string, std::string>& keywords,
	const std::multimap<std::string, std::string>& dictionary)
{
	std::mt19937 gen{ std::random_device{}() };
	const int count = 5;
	std::vector<std::pair<std::string, std::string>> first_words(count);
	std::sample(keywords.begin(), keywords.end(), first_words.begin(), count, gen);
	for (const auto& [word, definition] : first_words)
	{
		auto [second_word, offset] = find_overlapping_word_from_dictionary(word, dictionary, gen);
		if (second_word == "" || second_word == word)
		{
			std::cout << "couldn't find a match for " << word << '\n';
			continue; // TODO maybe remove keywords we don't match?
		}
		const auto& [lb, ub] = dictionary.equal_range(second_word);
		std::vector<std::pair<std::string, std::string>> dest(1); // TODO something better? maybe a view?
		std::sample(lb, ub, dest.begin(), 1, gen);
		std::string second_definition = dest[0].first;
		std::cout << definition << "\nAND\n" << second_definition << '\n';

		std::string answer = word.substr(0, offset) + second_word;

		std::string response;
		std::getline(std::cin, response);
		if (response == answer)
		{
			std::cout << "CORRECT!!!!!!!!!\n";
		}
		else
		{
			std::cout << answer << '\n';
		}
		std::cout << word << ' ' << second_word << "\n\n\n";
	}
}
void full_game()
{
	// raw strings and  std::filesystem::path. (since C++17)
	const auto dictionary = load_dictionary(R"(dictionary.csv)");
	const auto keywords = load_dictionary(R"(keywords.csv)");
	answer_smash(keywords, dictionary);
}

int main()
{
	check_properties();

	warm_up();
	std::cout << "\n\n";

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
	simple_answer_smash(keywords, dictionary);

	full_game();
}