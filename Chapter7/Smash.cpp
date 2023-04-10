#include <algorithm>
#include <fstream>
#include <iostream>
#include <random>

#include "Smash.h"

// Listing 7.9 Transform a string to lower case
// Based on https://en.cppreference.com/w/cpp/string/byte/tolower
std::string str_tolower(std::string s) {
	std::transform(s.begin(), s.end(), s.begin(),
		[](unsigned char c) { return std::tolower(c); }
	);
	return s;
}

// Listing 7.10 Load a file into a multimap
std::multimap<std::string, std::string> smashing::load_dictionary(const std::string& filename)
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
		// report error - TODO could throw instead
		std::cout << "Failed to open " << filename << '\n';
	}
	return dictionary;
}

// Listing 7.8 Find an overlapping word more efficiently
std::pair<std::string, int> smashing::find_overlapping_word(std::string word,
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

// Listing 7.6 Simple answer smash game
void smashing::simple_answer_smash(
	const std::map<std::string, std::string>& keywords,
	const std::map<std::string, std::string>& dictionary)
{
	for (const auto& [word, definition] : keywords)
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

// Listing 7.12 Better answer smash game
void smashing::answer_smash(
	const std::multimap<std::string, std::string>& keywords,
	const std::multimap<std::string, std::string>& dictionary)
{
	std::mt19937 gen{ std::random_device{}() };
	auto select_one = [&gen](auto lb, auto ub, auto dest) {
		std::sample(lb, ub, dest, 1, gen);
	};
	const int count = 5;
	std::vector<std::pair<std::string, std::string>> first_words;
	std::ranges::sample(keywords, std::back_inserter(first_words), count, gen);
	for (const auto& [word, definition] : first_words)
	{
		auto [second_word, second_definition, offset] = select_overlapping_word_from_dictionary(word, dictionary, select_one);
		if (second_word == "")
		{
			continue;
		}
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
