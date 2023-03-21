#include <cassert>
#include <iostream>
#include <random>
#include <string>
#include <type_traits>
#include <vector>

#include "Race.h"

void check_properties()
{
    // Listing 6.4 Check steps move a blob forward
    Race::StepperBlob blob;
    blob.step();
    assert(blob.steps() == 2);

    // Listing 6.10 Type traits to check for special member functions
    static_assert(!std::is_constructible<Race::Blob>::value); // 0 for ABC
    // alternatively use _v instead of ::value
    static_assert(!std::is_constructible_v<Race::Blob>); // 0 for ABC
    static_assert(std::is_destructible_v<Race::Blob>);
    static_assert(std::has_virtual_destructor_v<Race::Blob>);
    static_assert(!std::is_copy_constructible_v<Race::Blob>);
    static_assert(std::is_copy_assignable_v<Race::Blob>);
    static_assert(!std::is_move_constructible_v<Race::Blob>); // DOES NOT MEAN WE HAVE A MOVE CONSTRUCTOR, cos copy and be used instead
    static_assert(std::is_move_assignable_v<Race::Blob>);// DOES NOT MEAN WE HAVE MOVE ASSIGNMENT, cos copy and be used instead

    std::random_device rd;
    Race::RandomBlob rnd_blob{ 
        std::default_random_engine{ rd() },
        std::uniform_int_distribution{ 0, 4 }
    };
    rnd_blob.steps();
    assert(rnd_blob.steps() >= 0);

    Race::RandomBlob another_rnd_blob{
        std::default_random_engine{ rd() },
        std::poisson_distribution{ 2 }
    };
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

void do_race()
{
    using namespace Race;
    std::vector<std::unique_ptr<Blob>> blobs;
    const int number = 4;
    std::random_device rd;
    for (int i = 1; i < number; i += 2)
    {
        blobs.emplace_back(std::make_unique<StepperBlob>());
        blobs.emplace_back(std::make_unique<RandomBlob<std::default_random_engine, std::uniform_int_distribution<int>>>
            (std::default_random_engine{ rd() }, std::uniform_int_distribution{ 0, 4 })
        );
    }

    race(blobs);
}

int main()
{
    check_properties();

    // Listing 6.9 A warm up race
    std::vector<Race::StepperBlob> blobs(4);
    Race::race(blobs);
}
