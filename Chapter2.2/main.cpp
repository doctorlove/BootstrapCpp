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

auto get_next_row(const std::vector<int> & last_row)
{
    std::vector ret{ 1 };
    if (last_row.empty())
    {
        return ret;
    }

    for (unsigned idx = 0; idx+1 < last_row.size(); ++idx)
    {
        ret.emplace_back(last_row[idx] + last_row[idx + 1]);
    }
    ret.emplace_back(1);
    return ret;
}

auto generate_triangle(int rows)
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


template<typename T>
void show_triangle(const T& triangle)
{
    std::string space(triangle.back().size()/2, ' ');
    for (const auto row : triangle)
    {
        for (const auto data : row)
        {
            std::cout << data << space;
        }
        std::cout << '\n';
    }
}

template<typename T>
std::ostream& operator << (std::ostream & s, 
    const std::vector<std::vector<T>>& v)
{
    for (const auto& data : v)
    {
        std::ranges::copy(data, std::ostream_iterator<T>(s, " "));
        s << '\n';
    }
    return s;
}

// This code is slightly more general than the text of the book
//  since it takes a width of 6, allowing you to try more rows
//  Recall, 6 is fine for 16 or so rows. 
//  Once the entries are more than 4 digits the will overlap
void show_vectors(std::ostream& s,
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
void properties(const std::vector<std::vector<int>> & triangle)
{
    int expected_total = 1;
    int row_number = 1;
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


int main()
{
    auto triangle = generate_triangle(16); //Change 16 if you want

    // test triangle is correct
    properties(triangle);

    // display left justified
    std::cout << triangle;

    // Rather than sticking with a block of 6, we can find out how much we need
    auto biggest = std::max_element(triangle.back().begin(), triangle.back().end());
    auto width = std::to_string(*biggest);
    show_vectors(std::cout, triangle, width.size()+2);

    // Show odd numbers as stars
    show_view(std::cout, triangle);
}

