#pragma once

#include <concepts>
#include <memory>
#include <vector>

namespace Race
{
    // Listing 6.2 A base class
    class Blob
    {
    public:
        virtual ~Blob() = default;

        Blob() = default;
        Blob(Blob const&) = delete;
        Blob& operator=(Blob const&) = delete;

        virtual void step() = 0;
        virtual int total_steps() const = 0;
    };

    // Listing 6.3 A Blob taking constant sized steps
    // we noted in further considerations we could mark this as final
    class StepperBlob final : public Blob
    {
        int y = 0;
    public:
        void step() override
        {
            y += 2;
        }
        int total_steps() const override
        {
            return y;
        }
    };

    // Listing 6.11 A general purpose random blob
    // We started with temmplate-head
    // template <typename T, typename U>
    // then consdiered using the concept std::invocable<> 
    // for the generator in the text
    template <std::invocable<> T, typename U>
    class RandomBlob : public Blob
    {
        int y = 0;
        T generator;
        U distribution;
    public:
        RandomBlob(T gen, U dis)
            : generator(gen), distribution(dis)
        {

        }
        void step() override
        {
            y += static_cast<int>(distribution(generator));
        }
        int total_steps() const override { return y; }
    };

    void move_blobs(std::vector<Race::StepperBlob>& blobs);
    void draw_blobs(const std::vector<Race::StepperBlob>& blobs);
    void race(std::vector<Race::StepperBlob>& blobs);

    void race(std::vector<std::unique_ptr<Blob>>& blob);
    void move_blobs(std::vector<std::unique_ptr<Blob>>& blobs);
    void draw_blobs(const std::vector<std::unique_ptr<Blob>>& blob);
}