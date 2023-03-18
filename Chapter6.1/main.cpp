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
}

// Listing 6.8 A warm up race
void race_steppers()
{
    std::vector<Race::StepperBlob> blobs(4);
    const int max = 3;
    for (int i = 0; i < max; ++i)
    {
        Race::draw_blobs(blobs);
        Race::move_blobs(blobs);
    }
}

int main()
{
    check_properties();

    // Listing 6.9 A warm up race
    std::vector<Race::StepperBlob> blobs(4); 
    Race::race(blobs);
}

