#include <chrono>
#include <iostream>
#include <thread>

//Listing 4.2 Duration between two time points
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

//Listing 4.3 Use duration to move to a coarser representation
void durations()
{
    using namespace std::chrono;
    auto nearly_a_day = hours(23);
    days a_day = duration_cast<days>(nearly_a_day);
    hours round_trip = a_day;
    std::cout << a_day << '\n';
}

//Listing 4.4 Defining a duration
void defining_a_duration()
{
    using namespace std::chrono;
    using centuries = duration<long long,
        std::ratio_multiply<years::period, std::hecto>>;
// A type alias, which we used to write like this:
//    typedef duration<long long,
//        std::ratio_multiply<years::period, std::hecto>> centuries;
    centuries two_hunrdred_years = centuries(2);
    seconds sec = two_hunrdred_years;
    hours hrs = two_hunrdred_years;
    days day_count = duration_cast<days>(two_hunrdred_years);
}

//Listing 4.5 Writing and using a concept
// They are called in requirements_and_concepts below, but result in a compliation failure
// Try uncommenting the calls and see what happens
template<typename T>
concept Quacks = requires(T t)
{
    t.Quack();
};

template<typename T>
void might_be_a_duck(T x)
{
    x.Quack();
}

template<typename T>
requires Quacks<T>
void must_be_a_duck(T x)
{
    x.Quack();
}

void also_must_be_a_duck(Quacks auto x)
{
    x.Quack();
}

void requirements_and_concepts()
{
    //might_be_a_duck(42); // error left of '.Quack' must have class/struct/union
    //must_be_a_duck(42); //'f2': no matching overloaded function found, could be 'void f2(T)',  the associated constraints are not satisfied
    //also_must_be_a_duck(42);
}


//Listing 4.6 How many days until the end of the year?
void countdown()
{
    using namespace std::chrono;
    time_point now = system_clock::now();

    const auto ymd = year_month_day{
        floor<days>(now)
    };

    auto this_year = ymd.year();
    auto new_years_eve = this_year / December / 31;

    auto event = sys_days(new_years_eve);
    duration dur = event - now;
    std::cout << duration_cast<days>(dur)
        << " until event \n";
}

//Listing 4.7 A testable countdown
constexpr
std::chrono::system_clock::duration countdown(std::chrono::system_clock::time_point now)
{
    using namespace std::chrono;

    auto days_only = floor<days>(now);

    const auto ymd = year_month_day{days_only};

    auto this_year = ymd.year();
    auto new_years_eve = this_year / December / last;

    auto event = sys_days(new_years_eve);
    return event - now;
}

//Listing 4.8 Check the countdown function
void check_properties()
{
    using namespace std::chrono;
    constexpr auto new_years_eve = 2022y / December / last;
    constexpr auto one_day_away = sys_days{ new_years_eve } - 24h;
    constexpr auto result = countdown(one_day_away);
}

//Listing 4.10 Reading a date
std::optional<std::chrono::year_month_day> read_date(std::istream & in)
{
    using namespace std::string_literals;
    auto format_str = "%Y-%m-%d"s;
    std::chrono::year_month_day date;
    if (in >> std::chrono::parse(format_str, date))
    {
        return date;
    }
    else
    {
        in.clear();
        std::cout << std::format("Invalid format. Expected {}\n",
                format_str);
        return {};
    }
}

//Listing 4.11 Countdown to any event
constexpr std::chrono::system_clock::duration
countdown_to(std::chrono::system_clock::time_point now,
    std::chrono::year_month_day date)
{
    using namespace std::chrono;
    auto event = sys_days(date);
    return event - now;
}

//Listing 4.13 Countdown in local time
std::chrono::system_clock::duration
countdown_in_local_time(std::chrono::system_clock::time_point now,
    std::chrono::year_month_day date)
{
    using namespace std::chrono;
    auto local_now = zoned_time(current_zone(), now).get_local_time();
    auto event = local_days(date) + 0s;
    return event - local_now;
}

int main()
{
    duration_to_end_of_year();
    durations();
    defining_a_duration();
    countdown();

    std::cout << countdown(std::chrono::system_clock::now()) << " until event \n"; // calling listing 4.7
    check_properties();

    //Listing 4.9 Call the countdown in a loop
    using namespace std::chrono;
    for (int i = 0; i < 5; ++i)
    {
        std::this_thread::sleep_for(5000ms);
        auto dur = countdown(system_clock::now());
        std::cout << duration_cast<seconds>(dur) <<
            " until event\n";
    }

    //Listing 4.12 A general purpose countdown
    std::cout << "Enter a date\n>";
    std::string str;
    std::cin >> str;
    std::istringstream in(str);
    std::optional<std::chrono::year_month_day> event_date = read_date(in);
    if (event_date)
    {
        auto dur = countdown_to(system_clock::now(), event_date.value());
        std::cout << duration_cast<days>(dur) <<
            " until " << event_date.value() << "\n";

        // Calling the function from listing 4.13
        auto local_dur = countdown_in_local_time(system_clock::now(), event_date.value());
        std::cout << duration_cast<days>(local_dur) <<
            " until " << event_date.value() << "\n";

    }

}
