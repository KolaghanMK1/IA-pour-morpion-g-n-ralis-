#pragma once

#include "types.h"

/**
 * @file Gammatoe.h
 * @brief Header file for the Gammatoe AI black box implementation.
 *
 * Contains the Gammatoe class which implements a strategic AI player using
 * board evaluation. The class acts as a black box with two interfaces:
 * - INPUT: play_opponent() - provides opponent's move to update internal model
 * - OUTPUT: play() - returns AI's next move recommendation
 */

/**
 * @class Gammatoe
 * @brief Black box AI player for Gammatoe/Morpion game.
 *
 * This class implements a strategic game-playing AI that maintains an internal
 * board model. It operates as a black box with clear input/output interfaces:
 *
 * **INPUT:** Call play_opponent(x, y) to inform the AI of the opponent's move.
 * The AI updates its internal board model accordingly.
 *
 * **OUTPUT:** Call play() to request the AI's next move. The method returns
 * coordinates as a std::pair<int, int> in the format {x, y}.
 *
 * The AI evaluates board positions using a strategic scoring system that combines
 * offensive and defensive values, with emphasis on both creating winning opportunities
 * and blocking opponent threats.
 *
 * @note The AI assumes it plays as player=true (main player). Opponent plays as player=false.
 */
class Gammatoe
{
    Board board; ///< Internal board model tracking game state

public:
    /**
     * @brief Constructs a Gammatoe AI instance.
     *
     * Initializes the AI with an empty board and win condition.
     * The AI will play as player=true and opponent as player=false.
     *
     * @param size_ The size of the board (size_ × size_). Typical value: 20
     * @param k The number of pieces in a line needed to win. Typical value: 5
     *
     * @example
     * ```cpp
     * Gammatoe ai(20, 5);  // 20×20 board, 5-in-a-row to win
     * ```
     */
    Gammatoe(uint16_t size_, uint16_t k) : board(size_, k) {};

    /**
     * @brief INPUT: Record opponent's move in the internal board model.
     *
     * Call this method after the opponent plays to synchronize the AI's
     * internal model with the actual game state. This allows the AI to
     * maintain accurate board knowledge for strategic decision-making.
     *
     * @param x The x-coordinate of the opponent's move (0 to size-1)
     * @param y The y-coordinate of the opponent's move (0 to size-1)
     *
     * @pre Coordinates must be valid (inside board bounds)
     * @pre Cell at (x, y) must be empty
     *
     * @example
     * ```cpp
     * ai.play_opponent(10, 10);  // Opponent played at position (10, 10)
     * ```
     */
    void play_opponent(int x, int y);

    /**
     * @brief OUTPUT: Compute and return AI's next move.
     *
     * This is the main output method. It evaluates all empty cells on the board
     * and selects the move with the highest strategic value.
     *
     * **Evaluation Process:**
     * 1. For each empty cell, creates a temporary board copy
     * 2. Simulates placing an AI piece (player=true) at that location
     * 3. Updates the board to recalculate piece alignments and values
     * 4. Calculates the cell's strategic value: ai_general_value()
     * 5. Returns coordinates of the cell with maximum value
     *
     * **Strategic Scoring:**
     * - Offensive value: Identifies opportunities to create threats
     *   - Quasi-winning positions (almost complete lines): value = 16.0f
     *   - Strong sequences (3+ adjacent with gaps): value = 10.0f
     *   - General sequences: value = length × 3.0f
     * - Defensive value: Identifies opponent threats to block
     *   - Quasi-winning opponent positions: value = 19.0f (urgent)
     *   - Strong opponent sequences: value = 13.0f
     *   - General sequences: value = length × 6.0f
     * - **Combined Value:** 40% offensive + 60% defensive (defensive priority)
     *
     * After selecting the best move, the AI updates its internal board model
     * with the move so subsequent calls remain synchronized.
     *
     * @return std::pair<int, int> with first=x coordinate, second=y coordinate
     *         Represents the recommended move position
     *
     * @complexity O(size² × size²) - Evaluates every empty cell with board simulation
     *
     * @example
     * ```cpp
     * std::pair<int, int> move = ai.play();
     * int x = move.first;
     * int y = move.second;
     * std::cout << "AI plays at (" << x << ", " << y << ")" << std::endl;
     * ```
     *
     * @note The returned coordinates are automatically recorded in the internal
     *       board model, so the AI remains synchronized with its own moves.
     */
    std::pair<int, int> play();
};
