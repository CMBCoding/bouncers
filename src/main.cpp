#include <bn_core.h>
#include <bn_backdrop.h>
#include <bn_keypad.h>
#include <bn_sprite_ptr.h>
#include <bn_display.h>
#include <bn_random.h>
#include <bn_vector.h>
#include <bn_log.h>

#include "bn_sprite_items_dot.h"

// Set max/min x position to be the edges of the display
static constexpr int HALF_SCREEN_WIDTH = bn::display::width() / 2;
static constexpr bn::fixed MIN_X = -HALF_SCREEN_WIDTH;
static constexpr bn::fixed MAX_X = HALF_SCREEN_WIDTH;

static constexpr int HALF_SCREEN_HEIGHT = bn::display::height() / 2;
static constexpr bn::fixed MIN_Y = -HALF_SCREEN_HEIGHT;
static constexpr bn::fixed MAX_Y = HALF_SCREEN_HEIGHT;

// Starting speed of a bouncer
static constexpr bn::fixed BASE_SPEED = 2;

// Maximum number of bouncers on screen at once
static constexpr int MAX_BOUNCERS = 20;

class Bouncer
{
public:
    bn::sprite_ptr sprite; // Sprite
    bn::fixed x_speed;     // Speed of the sprite
    bn::fixed y_speed;     // Horizontal speed of the sprite

    // Constructor
    // Creates the sprite at the x and y
    // Create a dot sprite
    // Set the speed of the sprite to the base speed
    Bouncer(bn::fixed x_start, bn::fixed y_start) : sprite(bn::sprite_items::dot.create_sprite(x_start, y_start)),
                                                    x_speed(BASE_SPEED) {};

    // Method to get the x-coordinate of a bouncer
    bn::fixed get_x_pos()
    {
        bn::fixed x_pos = sprite.x();
        return x_pos;
    }

    void update()
    {
        bn::fixed x = sprite.x();
        // Update x position by adding speed

        // If we've gone off the screen on the right
        if (x > MAX_X)
        {
            // Snap back to screen and reverse direction
            x = MAX_X;
            x_speed *= -1;
        }
        // If we've gone off the screen on the left
        if (x < MIN_X)
        {
            // Snap back to screen and reverse direction
            x = MIN_X;
            x_speed *= -1;
        }

        x += x_speed;
        sprite.set_x(x);
    }

private:
};

bn::fixed average_x(bn::vector<Bouncer, MAX_BOUNCERS> &bouncers)
{
    // Add all x positions together
    bn::fixed x_sum = 0; // Start at 0
    for (Bouncer bouncer : bouncers)
    {
        // Add the position of the bouncer to x_sum
        x_sum += bouncer.get_x_pos();
    }
    bn::fixed x_average = x_sum;

    // Only divide if we have 1 or more
    // Prevents division by 0
    if (bouncers.size() > 0)
    {
        x_average /= bouncers.size();
    }

    return x_average;
};

void add_bouncer(bn::vector<Bouncer, MAX_BOUNCERS> &bouncers,
                 bn::fixed start_x, bn::fixed start_y)
{
    if (bouncers.size() < bouncers.max_size())
    {
        bouncers.push_back(Bouncer(start_x, start_y));
    };
}

int main()
{
    bn::core::init();

    // Sprites and x speeds of bouncers
    // Items with the same index correspond to each other
    // bn::vector<bn::sprite_ptr, MAX_BOUNCERS> sprites = {};
    // bn::vector<bn::fixed, MAX_BOUNCERS> x_speeds = {};

    // Create an instance of a bouncer
    bn::vector<Bouncer, MAX_BOUNCERS> bouncers = {};

    bn::fixed y_spawn = 0;

    // Bouncer manual = Bouncer(-30, 25);

    while (true)
    {
        if (bn::keypad::a_pressed())
        {
            add_bouncer(bouncers, 0, y_spawn);
        }

        for (Bouncer &b : bouncers)
        {
            b.update();
        }

        y_spawn += 1;

        if (y_spawn > MAX_Y)
        {
            y_spawn = MIN_Y;
        }

        // if A is pressed add a new bouncer
        // if (bn::keypad::a_pressed())
        // {
        //     // Only add if we're below the maximum
        //     if (sprites.size() < sprites.max_size())
        //     {
        //         sprites.push_back(bn::sprite_items::dot.create_sprite());
        //         x_speeds.push_back(BASE_SPEED);
        //     }
        // }

        // // if B is pressed print the average to the console
        if (bn::keypad::b_pressed())
        {
            bn::fixed x_average = average_x(bouncers);
            BN_LOG("Average x: ", x_average);
        }

        // for each bouncer
        // for (int i = 0; i < sprites.size(); i++)
        // {
        //     bn::sprite_ptr sprite = sprites[i];

        //     bn::fixed x = sprite.x();

        //     // Update x position by adding speed
        //     x += x_speeds[i];

        //     // If we've gone off the screen on the right
        //     if (x > MAX_X)
        //     {
        //         // Snap back to screen and reverse direction
        //         x = MAX_X;
        //         x_speeds[i] *= -1;
        //     }
        //     // If we've gone off the screen on the left
        //     if (x < MIN_X)
        //     {
        //         // Snap back to screen and reverse direction
        //         x = MIN_X;
        //         x_speeds[i] *= -1;
        //     }

        //     sprite.set_x(x);
        // }
        // manual.update();
        bn::core::update();
    }
}