#include <cassert>
#include <iostream>
#include <functional> // for hash int
#include <map>
#include <random>
#include <string>
#include <tuple> 
#include <utility>

void display(const std::map<std::tuple<int, int, int>, std::pair<int, int>>& history)
{
    for (const auto [k, v] : history)
    {
        auto [k1, k2, k3] = k;
        auto [v1, v2] = v;
        std::cout << k1 << ' ' << k2 << ' ' << k3 << ' ' << v1 << ' ' << v2 << ' ' << '\n';
    }
}

typedef std::tuple<int, int, int> key_t; // TODO or using?
// https://en.cppreference.com/w/cpp/utility/hash
// custom specialization of std::hash can be injected in namespace std
// also perfect hashes see e.g. 
//https://stackoverflow.com/questions/919612/mapping-two-integers-to-one-in-a-unique-and-deterministic-way
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

// also see https://stackoverflow.com/questions/7110301/generic-hash-for-tuples-in-unordered-map-unordered-set
// and https://stackoverflow.com/questions/26348301/hashing-function-for-three-signed-integers

class MindReader {
    // win, change, win etc
    // Listing 8.1
    std::unordered_map<std::tuple<int, int, int>, std::pair<int, int>> history // needs hash
        = {
            { {0,0,0}, {-1, -1} },
            { {0,0,1}, {-1, -1} },
            { {0,1,0}, {-1, -1} },
            { {0,1,1}, {-1, -1} },
            { {1,0,0}, {-1, -1} },
            { {1,0,1}, {-1, -1} },
            { {1,1,0}, {-1, -1} },
            { {1,1,1}, {-1, -1} },
    };

    std::mt19937 gen{ std::random_device{}() };
    std::uniform_int_distribution<int> dist{ 0, 1 };  // no CATD

    int prediction = flip();
    int previous2 = -1;
    int previous1 = -1;
    int previous_go = -1;

    int previous_turn_changed = -2;

public:
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

        if (previous2 != -1)
        {
            const auto [prev2, prev1] = history[{previous2, previous_turn_changed, previous1}];
            history[{previous2, previous_turn_changed, previous1}] = { prev1, turn_changed };
        }
        previous2 = previous1;
        previous1 = player_won_this_turn;

        if (previous2 > -1 and previous1 > -1) //might be able to ditch this - and on the 2nd go, we then have both in place but nothing to look up
        {
            const auto [changed_2_ago, changed_1_ago] = history[{previous2, previous_turn_changed, previous1}]; // will this add stuff if it's not there?
            if (changed_1_ago != -1 && changed_1_ago == changed_2_ago) // && and and, and flappy
            {
                prediction = changed_1_ago ? (player_choice ^ 1) : player_choice;
            }
            else
            {
                guessing = true;
            }
        }
        else
        {
            guessing = true;
            prediction = flip();
        }
        if (guessing)
        {
            prediction = flip();
        }
        previous_turn_changed = turn_changed;
        return guessing;
    }

    int flip()
    {
        return dist(gen);
    }
};


void check_properties()
{
    MindReader mr;
    assert( mr.update(0)); // guesses first
    assert( mr.update(0)); // second is a guess too

    // want to know all state is no change if I send in the same numbers
    // or always change if I always change numbers
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
    int guessing = 1; // first go is a guess

    std::cout << "Select 0 or 1 at random and press enter.\n";
    std::cout << "If the computer predicts your guess it wins\nand it can now read your mind.\n";

    MindReader mr;
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