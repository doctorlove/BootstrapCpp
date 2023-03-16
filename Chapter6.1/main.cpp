#include <cassert>
#include "Race.h"

void check_properties()
{
    Race::StepperBlob blob;
    blob.step();
    assert(blob.steps() == 2);
    Race::StepperBlob blob_copy = blob;
}

// Listing 6.5 A paper bag
#include <iostream>
#include <string>
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

int main()
{
    check_properties();
    draw_bag();
}
