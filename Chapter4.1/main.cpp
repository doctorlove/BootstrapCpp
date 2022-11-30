#include <chrono>
#include <iostream>

void duration_to_end_of_year()
{
    std::chrono::time_point now = std::chrono::system_clock::now();
    constexpr auto year = 2022; 
        auto new_years_eve = std::chrono::year_month_day(
            std::chrono::year(year),
            std::chrono::month(12),
            std::chrono::day(31)
        );
    auto event = std::chrono::sys_days(new_years_eve);
        std::chrono::duration dur = event - now;
        std::cout << dur << " until event\n";
}

int main()
{
    duration_to_end_of_year();
}