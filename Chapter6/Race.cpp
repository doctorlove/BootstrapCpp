#include <iostream>
#include <string>
#include <thread>

#include "Race.h"

// Listing 6.5 Draw each blob's current position
void Race::draw_blobs(const std::vector<Race::StepperBlob>& blobs)
{
    const int bag_height = 3;
    for (int y = 8; y >= 0; --y)
    {
        std::string output = y > 2 ? "  " : "| ";
        for (const auto& blob : blobs)
        {
            if (blob.total_steps() >= y)
            {
                output += "* ";
            }
            else
            {
                output += "  ";
            }
        }
        output += y >= bag_height ? ' ' : '|';
        std::cout << output << '\n';
    }
    std::cout << std::string(blobs.size() * 2 + 3, '-') << '\n';
}

// Listing 6.7 Move all the blobs
void Race::move_blobs(std::vector<Race::StepperBlob>& blobs)
{
    for (auto& blob : blobs)
    {
        blob.step();
    }
}

//  Listing 6.8 A somewhat predictable race
void Race::race(std::vector<Race::StepperBlob>& blobs)
{
    using namespace std::chrono;
    const int max = 3;
    std::cout << "\x1B[2J\x1B[H";
    for (int i = 0; i < max; ++i)
    {
        draw_blobs(blobs);
        move_blobs(blobs);
        std::this_thread::sleep_for(1000ms);
        std::cout << "\x1B[2J\x1B[H";
    }
    draw_blobs(blobs);
}

// Listing 6.15 A less predictable race
void Race::race(std::vector<std::unique_ptr<Blob>>& blobs)
{
    using namespace std::chrono;
    const int max = 3;
    std::cout << "\x1B[2J\x1B[H";
    for (int i = 0; i < max; ++i)
    {
        draw_blobs(blobs);
        move_blobs(blobs);
        std::this_thread::sleep_for(1000ms);
        std::cout << "\x1B[2J\x1B[H";
    }
    draw_blobs(blobs);
}

// Listing 6.16 Move all the blobs
void Race::move_blobs(std::vector<std::unique_ptr<Race::Blob>>& blobs)
{
    for (auto& blob : blobs)
    {
        blob->step();
    }
}

// Listing 6.17 Draw each blob's current position
void Race::draw_blobs(const std::vector<std::unique_ptr<Race::Blob>>& blobs)
{
    const int bag_height = 3;
    for (int y = 8; y >= 0; --y)
    {
        std::string output = y > 2 ? "  " : "| ";
        for (const auto& blob : blobs)
        {
            if (blob->total_steps() >= y)
                output += "* ";
            else
                output += "  ";
        }
        output += y >= bag_height ? ' ' : '|';
        std::cout << output << '\n';
    }
    std::cout << std::string(blobs.size() * 2 + 3, '-') << '\n';
}
