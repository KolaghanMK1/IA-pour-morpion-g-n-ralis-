#pragma once

#include "types.h"

// IA du morpion
class Gammatoe
{
    Board board;

public:
    // construit l'IA sur un plateau de taille size_
    Gammatoe(uint16_t size_, uint16_t k) : board(size_, k) {}

    // enregistre le coup de l'adversaire
    void play_opponent(int x, int y);

    // calcule le prochain coup de l'IA
    std::pair<int, int> play();
};
