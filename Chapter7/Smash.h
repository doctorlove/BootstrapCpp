#pragma once
#include <map>
#include <random>
#include <string>
#include <tuple>
#include <vector>

namespace smashing
{
	//template <std::invocable<> T> from #include <concepts> but samp,e checks for us
	template <typename T>
	std::tuple<std::string, std::string, int> select_overlapping_word_from_dictionary(std::string word,
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
				std::vector<std::pair<std::string, std::string>> dest;
				if (lb == ub)
					return std::make_tuple(lb->first, lb->second, offset);
				std::sample(lb, ub, std::back_inserter(dest), 1, gen); //since C++17
				auto found = dest[0].first;
				auto definition = dest[0].second;
				return std::make_tuple(found, definition, offset);
			}
			++offset;
		}
		return std::make_tuple("", "",  - 1);
	}

	std::pair<std::string, int> find_overlapping_word(std::string word,
		const std::map<std::string, std::string>& dictionary);
	void simple_answer_smash(
		const std::map<std::string, std::string>& keywords,
		const std::map<std::string, std::string>& dictionary);

	std::multimap<std::string, std::string> load_dictionary(const std::string& filename);
	void answer_smash(
		const std::multimap<std::string, std::string>& keywords,
		const std::multimap<std::string, std::string>& dictionary);
}
