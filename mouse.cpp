#include "mouse.h"

Mouse::Mouse(int w, int h)
{
    width = w;
    height = h;
}

void Mouse::setMouseProperties(sf::Vector2i vect)
{
    checkMouseOverScreen(vect);
    setMousePosition(vect);
    getRelativeMouseMovement(vect);
}

void Mouse::checkMouseOverScreen(sf::Vector2i vect)
{
    isOverScreen = false;
    if (0 < vect.x && vect.x < width)
    {
        if (0 < vect.y && vect.y < height)
        {
            isOverScreen = true;
        }
    }
}

void Mouse::setMousePosition(sf::Vector2i vect) {
    if (0 < vect.x && vect.x < width)
    {
        x = vect.x;
    }
    if (0 < vect.y && vect.y < height)
    {
        y = vect.y;
    }
}

void Mouse::getRelativeMouseMovement(sf::Vector2i vect)
{
    rel_x = vect.x - prev_x;
    rel_y = vect.y - prev_y;
    prev_x = vect.x;
    prev_y = vect.y;
}