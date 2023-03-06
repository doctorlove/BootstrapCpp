#pragma once

namespace Race
{
    // Listing 6.1 A base class
    class Blob
    {
    public:
        virtual ~Blob() = default;
        //prevent slicing and the need to define a default constructor by deleting the move assignment operator (Rule of DesDeMovA):
        Blob& operator=(Blob&&) = delete;
        virtual void step() = 0;
        int get_y() const { return y; }
    protected:
        int y = 0;
    };

    // Listing 6.2 A Blob taking constant sized steps
    class StepperBlob : public Blob
    {
    public:
        void step() override
        {
            y += 2;
        }
    };
}