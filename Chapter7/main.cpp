#include <algorithm>
#include <cassert>
#include <fstream>
#include <iostream>
#include <map>
#include <string>

// From https://en.cppreference.com/w/cpp/string/byte/tolower
std::string str_tolower(std::string s) {
	std::transform(s.begin(), s.end(), s.begin(),
		// static_cast<int(*)(int)>(std::tolower)         // wrong
		// [](int c){ return std::tolower(c); }           // wrong
		// [](char c){ return std::tolower(c); }          // wrong
		[](unsigned char c) { return std::tolower(c); } // correct
	);
	return s;
}


std::map<std::string, std::string> load_dictionary(const std::string& filename)
{
	std::map<std::string, std::string> dictionary;
	std::ifstream infile{ filename };
	if (infile)
	{
		// pull out to fn so I can test
		std::string line;
		//while (infile >> line) { // spilts on the first space in the definition
		while (std::getline(infile, line))
		{
			size_t position = line.find(',');
			if (position != std::string::npos)
			{
				std::string key{ line.substr(0, position) };
				std::string value{ line.substr(position + 1) };
				key = str_tolower(key);
				value = str_tolower(value);
				// might need a multimap
				if (dictionary.find(key) == dictionary.end())
					dictionary[key] = value;
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
std::pair<std::string, int> overlap(std::string word,
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
	auto [got1, offset1] = overlap("sprint", second);
	assert(got1 == "integer");
	auto [got2, offset2] = overlap("minus", second);
	assert(got2 == "struct");
	auto [got3, offset3] = overlap("vector", first);
	assert(got3 == "torch");

	auto [got4, offset4] = overlap("class", first);
	assert(got4 == "assault");
	std::string answer = "class" + got4.substr(offset4 + 1);
	assert(offset4 == 2);

	// what about full overlaps?
	// template plate
	//-> template

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
		auto [second_word, offset] = overlap(word, dictionary);
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

	//auto d = load_dictionary(R"(Chapter7\wordnetcode.txt)"); // note about raw strings
	auto d = load_dictionary(R"(wordnetcode.txt)");
	// then send in
	// need a function and maybe randomise which gets kept? Or maybe use a multimap?
	// also unordered maps
	// and a random sample of key words
	// maybe string_view for definition?
}