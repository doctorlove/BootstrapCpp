#include <cassert>
#include <iostream>
#include <string>
#include <vector>

#include "Race.h"

// Listing 6.4 Check steps move a blob forward
void check_properties()
{
    Race::StepperBlob blob;
    blob.step();
    assert(blob.steps() == 2);
    Race::StepperBlob blob_copy = blob;
}

// Listing 6.5 A paper bag
void draw_bag()
{
    const int bag_height = 3;
    const int blobs = 4;
    for (int y = 8; y >= 0; --y)
    {
        std::string output = y > 2 ? "  " : "| ";
        output += std::string(blobs*2, ' ');
        output += y >= bag_height ? ' ' : '|';
        std::cout << output << '\n';
    }
    std::cout << std::string(blobs*2 + 3, '-');
}

void draw_blobs(const std::vector<Race::StepperBlob> & blobs)
{
    const int bag_height = 3;
    for (int y = 8; y >= 0; --y)
    {
        std::string output = y > 2 ? "  " : "| ";
        for (auto& blob : blobs)
        {
            if (blob.steps() >= y)
                output += "* ";
            else
                output += "  ";
        }        
        output += y >= bag_height ? ' ' : '|';
        std::cout << output << '\n';
    }
    std::cout << std::string(blobs.size() * 2 + 3, '-');
}

// Listing 6.6 Move all the blobs
void move_blobs(std::vector<Race::StepperBlob> & blobs)
{
    for (auto& blob : blobs)
    {
        blob.step();
    }
}

void race_steppers()
{
    std::vector<Race::StepperBlob> blobs(4);
    const int max = 3;
    for (int i = 0; i < max; ++i)
    {
        draw_blobs(blobs);
        move_blobs(blobs);
    }
}

int main()
{
    check_properties();
    draw_bag();
    race_steppers();
}
