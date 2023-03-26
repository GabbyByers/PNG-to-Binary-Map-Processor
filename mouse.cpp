#include "mouse.h"

mouse::mouse(int width, int height)
{
    screen_width = width;
    screen_height = height;
}

void mouse::set_mouse_properties(sf::Vector2i vect)
{
    check_mouse_over_screen(vect);
    set_mouse_position(vect);
    get_relative_mouse_movement(vect);
}

void mouse::check_mouse_over_screen(sf::Vector2i vect)
{
    is_over_screen = false;
    if (0 < vect.x && vect.x < screen_width)
    {
        if (0 < vect.y && vect.y < screen_height)
        {
            is_over_screen = true;
        }
    }
}

void mouse::set_mouse_position(sf::Vector2i vect) {
    if (0 < vect.x && vect.x < screen_width)
    {
        x = vect.x;
    }
    if (0 < vect.y && vect.y < screen_height)
    {
        y = vect.y;
    }
}

void mouse::get_relative_mouse_movement(sf::Vector2i vect)
{
    rel_x = vect.x - prev_x;
    rel_y = vect.y - prev_y;
    prev_x = vect.x;
    prev_y = vect.y;
}