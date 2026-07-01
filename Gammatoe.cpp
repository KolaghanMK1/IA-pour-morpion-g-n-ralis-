/// @file Gammatoe_core.cpp
/// @brief Implementation of the Gammatoe AI black box player.

#include "Gammatoe.h"

// INPUT: Record opponent's move
void Gammatoe::play_opponent(int x, int y)
{
    std::cout << " opponent coo : (" << x << ", " << y << ")\n";
    assert(x >= 0 && y >= 0 && board.inside({static_cast<uint16_t>(x), static_cast<uint16_t>(y)}));
    // Place opponent piece (player=false)
    board.put_piece(false, {static_cast<uint16_t>(x), static_cast<uint16_t>(y)});
    // Update board state to recalculate axes and values
    board.update(false, {static_cast<uint16_t>(x), static_cast<uint16_t>(y)});
}

// OUTPUT: Compute AI's next move
std::pair<int, int> Gammatoe::play()
{
    coordinates coo;

    coordinates best_move_coo = {0, 0};
    float best_move_value = -std::numeric_limits<float>::infinity();
    for (uint16_t i = 0; i < board.size; i++)
    {
        for (uint16_t j = 0; j < board.size; j++)
        {
            if (board.at2D(i, j).free())
            {
                if (board.at2D(i, j).general_value() > best_move_value)
                {
                    best_move_coo = {i,j};
                    best_move_value = board.at2D(i, j).general_value();
                }
            }
        }
    }
    board.put_piece(true, best_move_coo);
    board.update(true, best_move_coo);
    return {best_move_coo.x, best_move_coo.y};
    
}