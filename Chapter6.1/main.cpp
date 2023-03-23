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
    rnd_blob.step();
    assert(rnd_blob.steps() >= 0);

    Race::RandomBlob another_rnd_blob{
        std::default_random_engine{ rd() },
        std::poisson_distribution{ 2 }
    };
    another_rnd_blob.step();
    assert(another_rnd_blob.steps() >= 0);

    // Listing 6.12 Testing with random generators and distributions
    Race::RandomBlob random_blob([]() { return 0; }, [](auto x) { return x(); });
    random_blob.step();
    assert(random_blob.steps() == 0);
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

// Listing 6.17 Create blobs for a proper race
std::vector<std::unique_ptr<Race::Blob>> create_blobs(int number)
{
    using namespace Race;
    std::vector<std::unique_ptr<Blob>> blobs;
    std::random_device rd;
    for (int i = 0; i < number; ++i)
    {
        blobs.emplace_back(std::make_unique<StepperBlob>());
        blobs.emplace_back(std::make_unique<RandomBlob<std::default_random_engine,
                                            std::uniform_int_distribution<int>>>
            (
                std::default_random_engine{ rd() },
                std::uniform_int_distribution{ 0, 4 }
            )
        );
    }
    return blobs;
}

int main()
{
    check_properties();

    // Running both races together might be confusing, so 
    // choose a type of race; either 6.9 (just steppers) or 6.18 (various types)

    // Listing 6.9 A warm up race
    //std::vector<Race::StepperBlob> blobs(4);
    //Race::race(blobs);

    // Listing 6.18 A proper race
    auto blobs = create_blobs(4);
    Race::race(blobs);
}
