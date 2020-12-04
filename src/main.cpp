#include <iostream>
#include "game.h"

int main() {
    std::cout << "2020 3GD Project Practicum" << std::endl;
    return Game::getInstance()->start();
}
