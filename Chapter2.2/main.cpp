#include <cassert>
#include <algorithm>
#include <iostream>
#include <iterator>
#include <format>
#include <functional>
#include <numeric>
#include <ranges>
#include <string>
#include <vector>

// In Visual Studio add /std:c++latest to 'properties | C++ | command line | additon options' to get the CTAD, ranges etc
// Or at least C++17 for --std in g++ (CTAD has been around since C++17)

//Listing 2.2 The next row of Pascal’s triangle using the previous row
std::vector<int> get_next_row(const std::vector<int>& last_row)
{
    std::vector ret{ 1 }; // CTAD - otherwise say std::vector<int>
    if (last_row.empty())
    {
        return ret;
    }

    for (size_t idx = 0; idx+1 < last_row.size(); ++idx)
    {
        ret.emplace_back(last_row[idx] + last_row[idx + 1]);
    }
    ret.emplace_back(1);
    return ret;
}

//Listing 2.3 Generating several rows of Pascal’s triangle
auto generate_triangle_first_listing(int rows)
{
    std::vector<int> data;
    std::vector<std::vector<int>> triangle;
    for (int row = 0; row < rows; ++row)
    {
        data = get_next_row(data);
        triangle.push_back(data);
    }
    return triangle;
}

//Listing 2.4 Moving a temporary
auto generate_triangle(int rows)
{
    std::vector<std::vector<int>> triangle{ {1} };
    for (int row = 1; row < rows; ++row)
    {
        triangle.emplace_back(get_next_row(triangle.back()));
    }
    return triangle;
}

//Listing 2.5 Sending the contents to a stream
template<typename T>
std::ostream& operator << (std::ostream & s, 
    const std::vector<std::vector<T>>& triangle)
{
    for (const auto& row : triangle)
    {
        std::ranges::copy(row, std::ostream_iterator<T>(s, " "));
        // If your compile doesn't support ranges, use std::copy instead:
        //std::copy(row.begin(), row.end(), std::ostream_iterator<T>(s, " "));
        s << '\n';
    }
    return s;
}

// Listing 2.7 Center justified output
void show_vectors(std::ostream& s,
    const std::vector<std::vector<int>>& v)
{
    size_t final_row_size = v.back().size();
    std::string spaces(final_row_size * 3, ' ');
    for (const auto& row : v)
    {
        s << spaces;
        if (spaces.size() > 3)
            spaces.resize(spaces.size() - 3);
        for (const auto& data : row)
        {
            s << std::format("{: ^{}}", data, 6);
        }
        s << '\n';
    }
}
// This code is slightly more general than the text of the book, shown above
//  since it takes a width of 6, allowing you to try more rows
//  Recall, 6 is fine for 16 or so rows. 
//  Once the entries are more than 4 digits the will overlap
void show_vectors_more_general(std::ostream& s,
    const std::vector<std::vector<int>>& v, size_t width = 6)
{
    const auto gaps  = width/2;
    std::string spaces(v.back().size() * gaps, ' ');
    for (const auto& row : v)
    {
        s << spaces;
        if (spaces.size() > gaps)
            spaces.resize(spaces.size()- gaps);
        for (const auto& data : row)
        {
            s << std::format("{: ^{}}", data, width);
        }
        s << '\n';
    }
}

// Based on
// https://en.cppreference.com/w/cpp/algorithm/ranges/equal
// contexpr not mentioned in the text
constexpr bool is_palindrome(const std::vector<int>& v)
{
    auto forward = v | std::views::take(v.size() / 2);
    auto backward = v | std::views::reverse | std::views::take(v.size() / 2);
    return std::ranges::equal(forward, backward);
}

// Some tests, using assert
// fails for a 36 row triangle - think about why
void check_properties(const std::vector<std::vector<int>> & triangle)
{
    int expected_total = 1;
    size_t row_number = 1;
    for (const auto & row : triangle)
    {
        assert(row.front() == 1);
        assert(row.back() == 1);
        assert(row.size() == row_number++);

        assert(std::accumulate(row.begin(),
            row.end(),
            0)
            == expected_total);

        expected_total *= 2;

        // symmetry
        assert(is_palindrome(row));

        auto negative = [](int x) { return x < 0; };
        auto negatives = row | std::views::filter(negative);
        assert(negatives.empty());
    }
}

//Listing 2.16 Show odd numbers as stars
void show_view(std::ostream& s,
    const std::vector<std::vector<int>>& v)
{
    const auto gaps = 1;
    std::string spaces(v.back().size() * gaps, ' ');
    for (const auto& row : v)
    {
        s << spaces;
        if (spaces.size() > gaps)
            spaces.resize(spaces.size() - gaps);
        auto odds = row | std::views::transform([](int x) { return x % 2 ? '*' : ' '; });
        for (const auto& data : odds)
        {
            s << data << ' ';
        }
        s << '\n';
    }
}


//Pulls together all the listing gradually added to main through the text
int main()
{
    auto triangle = generate_triangle(16); //Change 16 if you want

    // test triangle is correct
    check_properties(triangle);

    // display left justified
    std::cout << triangle;

    show_vectors(std::cout, triangle);

    // Rather than sticking with a block of 6, we can find out how much we need
    // for example if we have more rows
    auto biggest = std::max_element(triangle.back().begin(), triangle.back().end());
    auto width = std::to_string(*biggest);
    show_vectors_more_general(std::cout, triangle, width.size()+2);

    // Show odd numbers as stars
    show_view(std::cout, triangle);
}

