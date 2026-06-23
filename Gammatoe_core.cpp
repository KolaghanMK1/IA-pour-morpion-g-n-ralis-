/// @file Gammatoe_core.cpp
/// @brief Core AI implementation for the Gammatoe game.
///
/// Contains the Gammatoe class which implements the game logic with AI decision-making
/// using board evaluation and strategic move selection.

#include "types.h"

/**
 * @class Gammatoe
 * @brief Main game class implementing AI player logic.
 *
 * This class manages the game board and implements an AI strategy that evaluates
 * each possible move on the board and selects the one with the highest strategic value.
 */
class Gammatoe
{
    Board board; ///< The game board

public:
    /**
     * @brief Constructs a Gammatoe game instance.
     * @param size_ The size of the board (size_ x size_)
     * @param k The number of pieces in a line needed to win
     */
    Gammatoe(uint16_t size_, uint16_t k) : board(size_, k) {};

    /**
     * @brief Executes one AI move, selecting the best strategic position.
     *
     * This method evaluates all empty cells on the board:
     * 1. For each empty cell, simulates placing a player piece there
     * 2. Updates the board state to calculate the cell's strategic value
     * 3. Tracks the cell with the highest general value (40% offensive + 60% defensive)
     * 4. Places the piece at the best position and updates the main board
     *
     * Strategy favors:
     * - Offensive positions that can lead to three-in-a-row
     * - Defensive positions that block opponent threats
     * - Central positions near existing pieces
     */
    std::pair<int,int> play()
    {
        float best_value = -std::numeric_limits<float>::infinity();
        coordinates best_move;

        // Evaluate all empty cells
        for (uint16_t y = 0; y < board.size; ++y)
        {
            for (uint16_t x = 0; x < board.size; ++x)
            {
                coordinates c{x, y};

                // Skip occupied cells
                if (!board.at2D(c).free())
                    continue;

                // --- simulation: try placing piece at this position ---
                Board tmp = board;      // Create temporary board copy
                tmp.put_piece(true, c); // Place a player piece
                tmp.update(true, c);    // Update board state (create/merge/split axes)

                // Evaluate this cell's strategic value
                float value = tmp.at2D(c).ai_general_value();

                // Track the best move found so far
                if (value > best_value)
                {
                    best_value = value;
                    best_move = c;
                }
            }
        }

        // Execute the best move on the actual board
        board.put_piece(true, best_move);
        board.update(true, best_move);
        return {best_move.x,best_move.y};
    }

    void play_opponent(int x, int y)
    {
        assert( x >= 0 && y >= 0 && board.inside({x,y}));
        board.put_piece(false, {x,y});
        board.update(false, {x,y});
    }
};