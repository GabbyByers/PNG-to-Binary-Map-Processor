#include <SFML/Graphics.hpp>
#pragma once

class mouse {
public:
    int x = 0;
    int y = 0;
    int prev_x = 0;
    int prev_y = 0;
    int rel_x = 0;
    int rel_y = 0;
    int screen_width = 0;
    int screen_height = 0;
    bool is_over_screen = false;

    mouse(int width, int height);

    void set_mouse_properties(sf::Vector2i vect);

    void check_mouse_over_screen(sf::Vector2i vect);

    void set_mouse_position(sf::Vector2i vect);

    void get_relative_mouse_movement(sf::Vector2i vect);
};