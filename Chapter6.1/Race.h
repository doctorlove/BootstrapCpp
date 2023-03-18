#pragma once

#include <vector>

namespace Race
{
    // Listing 6.2 A base class
    class Blob
    {
    public:
        virtual ~Blob() = default;
        virtual void step() = 0;
        virtual int steps() const = 0;
    };

    // Listing 6.3 A Blob taking constant sized steps
    class StepperBlob : public Blob
    {
        int y = 0;
    public:
        void step() override
        {
            y += 2;
        }
        int steps() const override
        {
            return y;
        }
    };

    void move_blobs(std::vector<Race::StepperBlob>& blobs);
    void draw_blobs(const std::vector<Race::StepperBlob>& blobs);
    void race(std::vector<Race::StepperBlob>& blobs);
}