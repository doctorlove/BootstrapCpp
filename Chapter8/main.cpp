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
    std::uniform_int_distribution<int> dist{ 0, 1 };

    int prediction = flip();
    int previous2 = -1;
    int previous1 = -1;
    int previous_go = -1;

    int player_wins = 0;

    int previous_turn_changed = -2;

    int guessing = 0;

public:
    void update_history(int turn_changed)
    {
        const auto [prev2, prev1] = history[{previous2, previous_turn_changed, previous1}];
        history[{previous2, previous_turn_changed, previous1}] = { prev1, turn_changed };
    }

    void take_turn(int playerChoice)
    {
        int turn_changed = playerChoice != previous_go;
        previous_go = playerChoice;

        std::cout << "You pressed " << playerChoice << ", I guessed " << prediction << '\n';

        bool player_won_this_turn = playerChoice != prediction;
        if (player_won_this_turn) 
        {
            std::cout << " YOU WIN!\n";
            ++player_wins;
        }
        else
        {
            std::cout << " I WIN!\n";
        }

        if (previous2 != -1)
        {
            update_history(turn_changed);
        }
        previous2 = previous1;
        previous1 = player_won_this_turn;
        previous_turn_changed = turn_changed;

        if (previous2 > -1 and previous1 > -1)
        {
            const auto [prev2, prev1] = history[{previous2, previous_turn_changed, previous1}];
            if (prev1 == prev2)
            {
                prediction = prev1 ? (playerChoice ^ 1) : playerChoice;
            }
            else
            {
                ++guessing;
                prediction = flip();
            }
        }
        else
        {
            ++guessing;
            prediction = flip();
        }
    }

    int flip()
    {
        return dist(gen);
    }


    void update() 
    {
        std::string in;
        std::getline(std::cin, in);
        if (in == "0") 
        {
            take_turn(0);
        }
        else if (in == "1") 
        {
            take_turn(1);
        }
    }

    int get_player_wins()
    {
        return player_wins;
    }

    int get_times_guessed()
    {
        return guessing;
    }
};

// Lots of untidy, repeated code from previous class
class Guesser
{
    int player_wins = 0;
    int previous_go = -1;
    std::mt19937 gen{ std::random_device{}() };
    std::uniform_int_distribution<int> dist{ 0, 1 }; // no CTAD https://stackoverflow.com/questions/55026916/class-template-argument-deduction-in-member-variables
public:
    void take_turn(int playerChoice)
    {
        int prediction = dist(gen);
        int turn_changed = playerChoice != previous_go;
        previous_go = playerChoice;

        std::cout << "You pressed " << playerChoice << ", I guessed " << prediction << '\n';

        bool player_won_this_turn = playerChoice != prediction;
        if (player_won_this_turn) 
        {
            std::cout << " YOU WIN!\n";
            ++player_wins;
        }
        else 
        {
            std::cout << " I WIN!\n";
        }

    }

    void update()
    {
        std::string in;
        std::getline(std::cin, in);
        if (in == "0")
        {
            take_turn(0);
        }
        if (in == "1") 
        {
            take_turn(1);
        }
    }

    int get_player_wins() {
        return player_wins;
    }

};

void mind_reader()
{
    const int turns = 15; // might be better to have a way to stop
    std::cout << "Select 0 or 1 at random and press enter. If the computer predicts your guess it wins\n";
    Guesser g;
    for (int i = 0; i < turns; ++i)
    {
        g.update();
    }

    std::cout << "you win " << g.get_player_wins() << '\n';

    std::cout << "Select 0 or 1 at random and press enter. If the computer predicts your guess it wins\nand it can now read your mind.\n";
    MindReader mr;
    for (int i = 0; i < turns; ++i)
    {
        mr.update();
    }

    std::cout << "you win " << mr.get_player_wins() << '\n'
        << "machine guessed " << mr.get_times_guessed() << " times" << '\n'
        << "machine won " << (turns - mr.get_player_wins()) << '\n';

}


int main()
{
    mind_reader();
}