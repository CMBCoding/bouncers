#include <bn_core.h>
#include <bn_backdrop.h>
#include <bn_keypad.h>
#include <bn_sprite_ptr.h>
#include <bn_display.h>
#include <bn_random.h>
#include <bn_vector.h>
#include <bn_log.h>
#include "bn_sprite_items_dot.h"
#include "bn_random.h"              //Included to generate random numbers.
#include "bn_sprite_actions.h"      //Included to add spinning and looping.
#include "bn_regular_bg_ptr.h"      //Included to allow the use of background.
#include "bn_regular_bg_items_bg.h" //Included to allow the use of the bg background item.
#include "bn_math.h"                //Included to to allow the use of safe_degrees_angle in the spin function.
#include "bn_sound_items.h"

/* Import random to begin use of random generated numbers
◎ Use the random generated numbers to set the spawn coordinates of
each bouncer.
◎ Need to use the min and max values for the screen size. It is a
coordinate grid with the center at 0,0
    - We already have consexpr that set the bounds of the screen
        - These are our spawn boundaries.
◎ (Later) use the randoms to set the speed of each bouncer.

*/

// Initialize a random instance to use in the program.
bn::random gen;

//

// Set max/min x position to be the edges of the display.
static constexpr int HALF_SCREEN_WIDTH = bn::display::width() / 2;
static constexpr bn::fixed MIN_X = -HALF_SCREEN_WIDTH;
static constexpr bn::fixed MAX_X = HALF_SCREEN_WIDTH;

// Set the max/min y position to be the edges of the display.
static constexpr int HALF_SCREEN_HEIGHT = bn::display::height() / 2;
static constexpr bn::fixed MIN_Y = -HALF_SCREEN_HEIGHT;
static constexpr bn::fixed MAX_Y = HALF_SCREEN_HEIGHT;

// Random generator for the speed of the bouncers.
bn::fixed rand_speed = gen.get_fixed(1, 10);

// Method to create and return a random speed for the spawned bouncers
bn::fixed set_random_speed()
{
    // Randomly generate a speed between 1 and 10
    bn::fixed random_speed = gen.get_fixed(1, 11);
    BN_LOG("Random speed is :", random_speed);
    return random_speed;
};

// Starting speed of a bouncer
bn::fixed BASE_SPEED = 1;

// Maximum number of bouncers on screen at once
static constexpr int MAX_BOUNCERS = 20;

class Bouncer
{
public:
    bn::sprite_ptr sprite;    // Sprite
    bn::fixed x_speed;        // Speed of the sprite
    bn::fixed y_speed;        // Horizontal speed of the sprite
    bn::fixed spin_angle = 0; // Set the intial angle of the bouncer

    // Constructor
    // Creates the sprite at the x and y
    // Create a dot sprite
    // Set the speed of the sprite to the base speed
    Bouncer(bn::fixed x_start, bn::fixed y_start) : sprite(bn::sprite_items::dot.create_sprite(x_start, y_start)),
                                                    x_speed(BASE_SPEED = set_random_speed()), y_speed(BASE_SPEED = set_random_speed()) {};

    // Method to get the x-coordinate of a bouncer
    bn::fixed get_x_pos()
    {
        bn::fixed x_pos = sprite.x();
        return x_pos;
    };

    // Method to get the sprite out of a bouncer.
    bn::sprite_ptr get_sprite()
    {
        return sprite;
    }

    // Method to get the y-coordinate of a bouncer
    bn::fixed get_y_pos()
    {
        bn::fixed y_pos = sprite.y();
        return y_pos;
    }

    /* Method to update the x and y coordinates of the bouncer, its speed at both angles
    and the actions to take if it meets the bounds of the screen.*/
    void update()
    {
        bn::fixed x = sprite.x();
        // Update x position by adding speed

        // Get the y position to use the updates
        bn::fixed y = sprite.y();

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

        // If a bouncer ball excedes the bounds of the top of the screen...
        if (y > MAX_Y)
        {
            // Pull the ball back within the screen's bounds
            y = MAX_Y;
            // Send it in the opposite direction by multiplying it with -1
            y_speed *= -1;
        }

        if (y < MIN_Y)
        {
            // Pull the ball back within the screen's bounds
            y = MIN_Y;
            // Send it in the opposite direction by multiplying it with -1
            y_speed *= -1;
        }

        y += y_speed;    // Increase the sprite's y-position. by the value of y_speed.
        sprite.set_y(y); // Set the sprite's y-position.
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

// Method that adds bouncers to the prexisting bouncers vector.
void add_bouncer(bn::vector<Bouncer, MAX_BOUNCERS> &bouncers,
                 bn::fixed start_x, bn::fixed start_y)
{
    if (bouncers.size() < bouncers.max_size())
    {
        bouncers.push_back(Bouncer(start_x, start_y));
        // Play the spawn sound effect.
        bn::sound_items::gen.play();
    };
}

// Method to spin the dot using the spin_angle variable
void spin(bn::vector<Bouncer, MAX_BOUNCERS> &bouncers)
{
    // For every bouncer within bouncers
    for (Bouncer &b : bouncers)
    {
        // Increment the spin angle of the bouncer
        b.spin_angle += 5;

        /* Set the rotation to a safe integer by using safe_degrees_angle
        Sending spin_angle to that function ensures that the number returned is always between
        0 - 360. */
        b.get_sprite().set_rotation_angle(bn::safe_degrees_angle(b.spin_angle));
    }
}

int main()
{
    bn::core::init();
    // Create a background and place it at the screen's center
    bn::regular_bg_ptr bg = bn::regular_bg_items::bg.create_bg(0, 0);

    // Create an instance of a bouncer
    bn::vector<Bouncer, MAX_BOUNCERS> bouncers = {};

    while (true)
    {
        if (bn::keypad::a_pressed())
        {
            // Generate a random spawn for the bouncers within the screen's bounds.
            bn::fixed spawn_x = gen.get_fixed(MIN_X, MAX_X);
            bn::fixed spawn_y = gen.get_fixed((MIN_Y + 8), (MAX_Y - 8));

            // Use those spawns to populate the screen with bouncers.
            add_bouncer(bouncers, spawn_x, spawn_y);
        }

        // Call the spin function on the bouncers.
        spin(bouncers);

        // For each bouncer on in the vector, call the update function.
        for (Bouncer &b : bouncers)
        {
            b.update();
        }

        // // if B is pressed print the average to the console
        if (bn::keypad::b_pressed())
        {
            bn::fixed x_average = average_x(bouncers);
            BN_LOG("Average x: ", x_average);
        }

        bn::core::update();
    }
}