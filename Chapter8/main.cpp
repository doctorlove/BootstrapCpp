#include <cassert>
#include <concepts>
#include <iostream>
#include <functional> // for hash int
#include <map>
#include <random>
#include <string>
#include <tuple> 
#include <utility>

using key_t = std::tuple<int, int, int>; // c.f. typedef
using value_t = std::pair<int, int>;

// https://en.cppreference.com/w/cpp/utility/hash
// custom specialization of std::hash can be injected in namespace std
template<>
struct std::hash<key_t>
{
    std::size_t operator()(key_t const& k) const noexcept
    {
        std::size_t h1 = std::hash<int>{}(std::get<0>(k));
        std::size_t h2 = std::hash<int>{}(std::get<1>(k));
        std::size_t h3 = std::hash<int>{}(std::get<2>(k));
        return h1 ^ (h2 << 1) ^ (h3 << 2); // or use boost::hash_combine
    }
};

// Listing 8.1
// TODO change to function in text
std::unordered_map<key_t, value_t> initial_state()
{
    return {
        { {0,0,0}, {-1, -1} },
        { {0,0,1}, {-1, -1} },
        { {0,1,0}, {-1, -1} },
        { {0,1,1}, {-1, -1} },
        { {1,0,0}, {-1, -1} },
        { {1,0,1}, {-1, -1} },
        { {1,1,0}, {-1, -1} },
        { {1,1,1}, {-1, -1} },
    };
}


enum class Action
{
    Same,
    Change,
    Shrug,
};

Action prediction_method(const value_t& choices)
{
    auto action = Action::Shrug;
    if (choices.first != -1 && choices.second != -1
        && choices.first == choices.second)
    {
        action = choices.first ? Action::Change : Action::Same;
    }

    return action;
}

class State
{
    std::unordered_map<key_t, value_t> state = initial_state();
public:
    value_t choices(const key_t& key)
    {
        if (state.contains(key)) // can we do a find instead?
        {
            return state[key];
        }
        else
        {
            return { -1, -1 };
        }
    }

    void update(const key_t& key, const value_t& value)
    {
        if (state.contains(key))
        {
            if (std::get<0>(key) != -1) // might not need to check this rest but this is fragile
            {
                state[key] = value;
            }
        }
        //else // TODO talk about this
        //{
        //    std::cout << "No such key " << std::get<0>(key) << std::get<1>(key) << std::get<2>(key) << "\n";
        //    //    throw std::invalid_argument("key invalid"); // first couple of goes will have -1s in
        //}
    }
};

template <std::invocable<> T, typename U>
class MindReader {
    State state_table;

    T generator;
    U distribution;

    int prediction = flip();
    int previous2 = -1;
    int previous1 = -1;
    int previous_go = -1;

    int previous_turn_changed = -1;

public:
    MindReader(T gen, U dis)
        : generator(gen), distribution(dis)
    {
    }

    int get_prediction()
    {
        return prediction;
    }

    bool update(int player_choice)
    {
        bool guessing = false;
        const int turn_changed = player_choice != previous_go;
        previous_go = player_choice;

        const bool player_won_this_turn = player_choice != prediction;

        // two lines not very encapsulated
        const auto [prev2, prev1] = state_table.choices({ previous2, previous_turn_changed, previous1 });
        state_table.update({ previous2, previous_turn_changed, previous1 }, { prev1, turn_changed });

        previous2 = previous1;
        previous_turn_changed = turn_changed;
        previous1 = player_won_this_turn;

        auto option = prediction_method(state_table.choices({ previous2, previous_turn_changed, previous1 }));
        switch (option)
        {
        case Action::Shrug:
            prediction = flip();
            guessing = true;
            break;
        case Action::Change:
            prediction = player_choice ^ 1;
            break;
        case Action::Same:
            prediction = player_choice;
            break;
        }
        return guessing;
    }

    int flip()
    {
        return distribution(generator);
    }
};


void check_properties()
{
    // No bucket clashes
    std::unordered_map<std::tuple<int, int, int>, value_t> history = initial_state();
    assert(history.size() == history.bucket_count());

    {
        MindReader mr([]() { return 0; }, [](auto gen) { return gen(); });
        assert(mr.update(0)); // guesses first
        assert(mr.update(0)); // second is a guess too
    }

    assert(prediction_method({ -1, -1 }) == Action::Shrug);
    assert(prediction_method({ -1, 0 }) == Action::Shrug);
    assert(prediction_method({ -1, 1 }) == Action::Shrug);
    assert(prediction_method({ 0, -1 }) == Action::Shrug);
    assert(prediction_method({ 1, -1 }) == Action::Shrug);
    assert(prediction_method({ 0, 0 }) == Action::Same);
    assert(prediction_method({ 1, 1 }) == Action::Change);
    assert(prediction_method({ 0, 1 }) == Action::Shrug);
    assert(prediction_method({ 1, 0 }) == Action::Shrug);

    State s;
    auto got1 = s.choices({ -1, -1, -1 });
    assert(got1.first == -1);
    assert(got1.second == -1);
    auto got2 = s.choices({ 0, 0, -1 });
    assert(got2.first == -1);
    assert(got2.second == -1);
    auto got3 = s.choices({ 1, 1, -1 });
    assert(got3.first == -1);
    assert(got3.second == -1);

    {
        // Listing 6.12 had a RandomBlob we tested
        // This always returns 0
        MindReader mr([]() { return 0; }, [](auto gen) { return gen(); });
        assert(mr.update(0)); //flip first two goes
        assert(mr.update(0)); //flip first two goes
        // The random generator always returns zero
        // it guesses first
        // state is 
        // lose, same, lose
        // but without two matching next choices
        assert(mr.update(0));
        // now 
        // lose, same, lose -> -1 ,lose
        // so when we decide 0 it stops guessing
        // now the state is
        // lose, same, lose -> lose ,lose
        // so it will predict a 0 next
        assert(!mr.update(0));
        assert(mr.get_prediction() == 0);
        assert(!mr.update(0));
        assert(mr.get_prediction() == 0);
        assert(!mr.update(0));
        assert(mr.get_prediction() == 0);
        assert(!mr.update(0));
        assert(mr.get_prediction() == 0);
        assert(!mr.update(0));
        assert(mr.get_prediction() == 0);
    }
}

// Listing 8.1 Read an optional zero or one
std::optional<int> read_number(std::istream& in)
{
    std::string line;
    std::getline(std::cin, line);
    if (line == "0") {
        return { 0 };
    }
    else if (line == "1") {
        return { 1 };
    }
    return {};
}

// Listing 8.2 A pennies game
void pennies_game()
{
    int player_wins = 0;
    int turns = 0;
    std::mt19937 gen{ std::random_device{}() };
    std::uniform_int_distribution dist(0, 1);

    std::cout << "Select 0 or 1 at random and press enter.\n";
    std::cout << "If the computer predicts your guess it wins.\n";
    while (true)
    {
        const int prediction = dist(gen);

        auto input = read_number(std::cin);
        if (!input)
        {
            break;
        }
        int player_choice = input.value();

        ++turns;
        std::cout << "You pressed " << player_choice << ", I guessed " << prediction << '\n';

        if (player_choice != prediction)
        {
            ++player_wins;
        }
    }
    std::cout << "you win " << player_wins << '\n'
        << "I win " << turns - player_wins << '\n';
}

void mind_reader()
{
    int turns = 0;
    int player_wins = 0;
    int guessing = 0;

    std::cout << "Select 0 or 1 at random and press enter.\n";
    std::cout << "If the computer predicts your guess it wins\nand it can now read your mind.\n";

    std::mt19937 gen{ std::random_device{}() };
    std::uniform_int_distribution dist{ 0, 1 };

    MindReader mr(gen, dist);
    while (true)
    {
        auto input = read_number(std::cin);
        if (!input)
        {
            break;
        }
        ++turns;
        int player_choice = input.value();

        int prediction = mr.get_prediction();
        std::cout << "You pressed " << player_choice << ", I guessed " << prediction << '\n';

        if (player_choice != prediction)
        {
            std::cout << " YOU WIN!\n";
            ++player_wins;
        }
        else
        {
            std::cout << " I WIN!\n";
        }
        if (mr.update(player_choice))
        {
            ++guessing;
        }
    }
    std::cout << "you win " << player_wins << '\n'
        << "machine guessed " << guessing << " times" << '\n'
        << "machine won " << (turns - player_wins) << '\n';
}


int main()
{
    pennies_game();
    check_properties();
    mind_reader();
}