#pragma once
/// @file types.h
/// @brief Core data structures for game board, pieces, and axes management.
///
/// Defines fundamental structures for Gomoku/Morpion game: board representation,
/// game pieces, coordinate system, and board update mechanisms. Provides core
/// infrastructure for game logic and AI evaluation.

#include <iostream>
#include <cstdint> 
#include <vector>
#include <array>
#include <list>
#include <set>
#include <utility>
#include <memory>
#include <cassert>
#include <optional>
#include <limits>
#include <math.h>

/**
 * @struct coordinates
 * @brief Represents a 2D position on the game board.
 *
 * Stores x and y coordinates as unsigned 16-bit integers and provides
 * lexicographic comparison operators for coordinate ordering and equality checks.
 * Coordinates are 0-indexed from the top-left corner (0,0) to bottom-right (size-1, size-1).
 *
 * @details Coordinates use lexicographic ordering: first compare x values, then y values.
 * This ordering is used for axis endpoints and piece identification.
 */
struct coordinates
{
    uint16_t x; ///< X-coordinate (column position, 0 = leftmost)
    uint16_t y; ///< Y-coordinate (row position, 0 = topmost)

    /**
     * @brief Constructs a coordinates object with specified or default values.
     * @param x The x-coordinate (column position). Default: 0
     * @param y The y-coordinate (row position). Default: 0
     */
    coordinates(uint16_t x = 0, uint16_t y = 0);

    /**
     * @brief Lexicographic less-than comparison.
     * Compares x-coordinates first; if equal, compares y-coordinates.
     * @param other The coordinates to compare against
     * @return true if this coordinate is lexicographically less than other
     */
    bool operator<(const coordinates &other) const;

    /**
     * @brief Lexicographic greater-than comparison.
     * Compares x-coordinates first; if equal, compares y-coordinates.
     * @param other The coordinates to compare against
     * @return true if this coordinate is lexicographically greater than other
     */
    bool operator>(const coordinates &other) const;

    /**
     * @brief Equality comparison operator.
     * @param other The coordinates to compare against
     * @return true if both x and y coordinates are equal
     */
    bool operator==(const coordinates &other) const;

    /**
     * @brief Inequality comparison operator.
     * @param other The coordinates to compare against
     * @return true if either x or y coordinates are not equal
     */
    bool operator!=(const coordinates &other) const;

    /**
     * @brief Lexicographic less-than-or-equal comparison.
     * Compares x-coordinates first; if equal, compares y-coordinates.
     * @param other The coordinates to compare against
     * @return true if this coordinate is lexicographically less than or equal to other
     */
    bool operator<=(const coordinates &other) const;

    /**
     * @brief Lexicographic greater-than-or-equal comparison.
     * Compares x-coordinates first; if equal, compares y-coordinates.
     * @param other The coordinates to compare against
     * @return true if this coordinate is lexicographically greater than or equal to other
     */
    bool operator>=(const coordinates &other) const;
};

/**
 * @enum direction
 * @brief Represents the four cardinal directions on the game board.
 *
 * Defines the four possible alignment directions for pieces: horizontal, vertical,
 * and two diagonal directions. These directions are used to organize and track
 * piece sequences (axes) and to determine strategic moves and win conditions.
 *
 * @details Each direction has a numerical index for array-based storage in pieces.
 */
enum class direction
{
    H = 0,  ///< Horizontal direction: left-right along the x-axis
    V = 1,  ///< Vertical direction: up-down along the y-axis
    D1 = 2, ///< Diagonal direction: top-left ↘ to bottom-right
    D2 = 3  ///< Diagonal direction: top-right ↙ to bottom-left
};

/// @brief Forward declaration of piece struct (defined later in file)
struct piece;

/**
 * @struct cell
 * @brief Represents a single cell on the game board.
 *
 * Each cell may contain a game piece or be empty. Provides comprehensive evaluation
 * methods for AI decision-making with both offensive and defensive value calculations.
 * Cells track strategic value metrics and machine learning bonus values for enhanced
 * move selection.
 *
 * @details Values are computed based on the piece configurations in all four directions
 * and updated whenever the board state changes.
 */
struct cell
{
    std::shared_ptr<piece> pc; ///< Shared pointer to piece in this cell (null if empty)

    /**
     * @brief Checks if this cell is free (unoccupied).
     *
     * @return true if cell contains no piece or piece doesn't exist, false otherwise
     */
    bool free() const;

    /// @brief Strategic offensive value: how good this move is for creating threats
    float offensive_value = 0.0;

    /// @brief Strategic defensive value: importance of blocking opponent threats
    float defensive_value = 0.0;

    std::array<std::pair<bool,float>,4> axis_values{{{false,0.0f}}};

    /**
     * @brief Computes combined strategic value of the cell.
     *
     * Combines offensive and defensive values with weighted priority (60% defensive, 40% offensive).
     * Handles infinity values for quasi-winning moves.
     *
     * @return Combined strategic value; infinity if move creates immediate win threat
     */
    float general_value() const;

    /**
     * @brief Updates the cell's offensive and defensive values.
     *
     * Recalculates strategic values based on current board state and piece configurations
     * in all four directions from this cell.
     *
     * @param coo The coordinates of this cell
     */
    void update_values(const direction d, const bool player, const float value);

    /// @brief Default constructor initializing empty cell with zero bonuses
    cell() = default;

    /**
     * @brief Constructs a cell with an initial piece.
     *
     * @param pc_ Unique pointer to the piece to place in this cell
     */
    cell(std::unique_ptr<piece> pc_);
};

struct axis;

/**
 * @struct Board
 * @brief Manages the complete game board state and all board operations.
 *
 * Central data structure that maintains all game cells in row-major 1D storage,
 * manages piece alignments through axis structures, handles piece placement,
 * and updates board state dynamically. Provides coordinate access, directional
 * navigation, and strategic evaluation infrastructure for the AI system.
 *
 * @details The board uses 0-indexed coordinates (0,0) at top-left corner.
 * All board updates trigger automatic axis recalculation and value updates.
 */
struct Board
{
    /// @brief All board cells stored in 1D row-major order (index = y * size + x)
    std::vector<cell> board;

    /// @brief Board dimensions (square board of size × size)
    uint16_t size;

    /// @brief Win condition: number of aligned pieces required for victory
    uint16_t number_piece_to_align_to_win;

    /// @brief All axes (piece alignments) on the board organized by direction
    std::list<std::shared_ptr<axis>> axes;


    /**
     * @brief Constructs a Board with specified dimensions and win condition.
     * @param s The board size (creates s × s board)
     * @param k The number of pieces to align to win
     */
    Board(uint16_t s, uint16_t k) : board(s * s), size(s), number_piece_to_align_to_win(k) {}

    /**
     * @brief Checks if coordinates are within board boundaries.
     * @param coo The coordinates to check
     * @return true if coordinates are valid (0 <= x,y < size), false otherwise
     */
    bool inside(coordinates coo) const
    {
        return coo.x < size && coo.y < size;
    }

    /**
     * @brief Gets a cell by its 1D index (no bounds checking).
     * @param i The 1D index (0 to size*size-1)
     * @return Reference to the cell at index i
     */
    cell &operator[](int i)
    {
        return board[i];
    }

    /**
     * @brief Gets a cell by its 1D index with bounds checking.
     * @param i The 1D index
     * @return Reference to the cell at index i
     * @throws std::out_of_range if index is out of bounds
     */
    cell &at1D(int i)
    {
        return board.at(i);
    }

    /// @brief Const version of at1D
    const cell &at1D(int i) const
    {
        return board.at(i);
    }

    /**
     * @brief Gets a cell by its 2D coordinates with bounds checking via assert.
     * @param x The x-coordinate (column)
     * @param y The y-coordinate (row)
     * @return Reference to the cell at (x, y)
     */
    cell &at2D(int x, int y)
    {
        assert(y < size);
        assert(x < size);
        return board.at(y * size + x);
    }

    /// @brief Const version of at2D(int, int)
    const cell &at2D(int x, int y) const
    {
        assert(y < size);
        assert(x < size);
        return board.at(y * size + x);
    }

    /**
     * @brief Gets a cell by coordinates object with bounds checking.
     * @param coo The coordinates {x, y}
     * @return Reference to the cell at coordinates
     */
    cell &at2D(coordinates coo)
    {
        return at2D(coo.x, coo.y);
    }

    /// @brief Const version of at2D(coordinates)
    const cell &at2D(coordinates coo) const
    {
        return at2D(coo.x, coo.y);
    }

    /**
     * @brief Calculates the Chebyshev distance (maximum of horizontal and vertical distance).
     * @param cooA First coordinates
     * @param cooB Second coordinates
     * @return The maximum of |x1-x2| and |y1-y2|
     */
    uint16_t distance(coordinates cooA, coordinates cooB)
    {
        return std::max(std::max(cooA.x, cooB.x) - std::min(cooA.x, cooB.x), std::max(cooA.y, cooB.y) - std::min(cooA.y, cooB.y));
    }

    /**
     * @brief Determines the direction from one coordinate to another.
     *
     * Identifies alignment: horizontal (H), vertical (V), or diagonal (D1, D2).
     *
     * @param A First coordinates
     * @param B Second coordinates
     * @return The direction enum (H, V, D1, or D2) connecting A to B
     * @pre Coordinates must be aligned in one of the four directions
     */
    direction get_direction(const coordinates A, const coordinates B);

    /**
     * @brief Navigates one cell backward (previous) in a given direction.
     *
     * @param d The direction to move backward along
     * @param coo Current coordinates
     * @return Optional coordinates of previous cell; std::nullopt if at boundary
     */
    std::optional<coordinates> prev(const direction d, coordinates coo);

    /**
     * @brief Navigates one cell forward (next) in a given direction.
     *
     * @param d The direction to move forward along
     * @param coo Current coordinates
     * @return Optional coordinates of next cell; std::nullopt if at boundary
     */
    std::optional<coordinates> next(const direction d, coordinates coo);

    /**
     * @brief Jumps multiple cells in a specified direction.
     *
     * @param d The direction to move along (H, V, D1, D2)
     * @param way true to move forward, false to move backward
     * @param start Starting coordinates
     * @param distance Number of cells to jump (>= 1)
     * @return Optional coordinates after jumping; std::nullopt if boundary exceeded
     */
    std::optional<coordinates> jump(const direction d, const bool way, const coordinates start, uint16_t distance);

    /**
     * @brief Creates a new axis connecting two pieces in the same direction.
     *
     * Registers a new axis with the board and updates both pieces' axis pointers.
     *
     * @param player true for player's axis, false for opponent's axis
     * @param A First endpoint coordinates (must contain a piece)
     * @param B Second endpoint coordinates (must contain a piece)
     * @pre Both cells must contain pieces and must be aligned
     */
    void create_axis(const bool player, const coordinates A, const coordinates B);

    /**
     * @brief Removes an axis from the board and axis list.
     *
     * @param axe Shared pointer to the axis to remove
     */
    void remove_axis(std::shared_ptr<axis> axe);

    /**
     * @brief Merges two adjacent axes into a single continuous axis.
     *
     * Called when a new piece bridges a gap between two existing axes.
     *
     * @param wcurrent Weak pointer to the current (first) axis
     * @param wother Weak pointer to the axis to merge into current
     */
    void merge_axis(std::weak_ptr<axis> wcurrent, std::weak_ptr<axis> wother);

    /**
     * @brief Splits an axis when a piece placed within it creates a gap.
     *
     * Creates a new axis segment when an occupied cell is discovered within an existing axis.
     *
     * @param waxis Weak pointer to the axis containing the split point
     * @param coo The coordinates where the split occurs (occupied cell)
     */
    void split_axis(std::weak_ptr<axis> waxis, const coordinates coo);

    /**
     * @brief Places a piece on the board at given coordinates.
     *
     * Creates and registers a new piece in the specified cell.
     * Does not update axes; call board.update() separately.
     *
     * @param player true for player's piece, false for opponent's piece
     * @param coo The coordinates where to place the piece
     * @pre Cell at coo must be empty (asserted)
     */
    void put_piece(const bool player, const coordinates coo);

    /**
     * @brief Recalculates strategic values for all cells based on current board state.
     *
     * Updates offensive and defensive values for all empty cells based on piece configurations.
     */
    void update_cells_values();

    /**
     * @brief Updates board state after a piece is placed.
     *
     * Orchestrates complete board update: creates new axes, merges adjacent axes,
     * splits axes at gaps, updates piece statistics, and recalculates cell values.
     * Handles complex axis topology changes resulting from the new piece.
     *
     * @param player true if player's piece was placed, false for opponent
     * @param coo The coordinates of the newly placed piece
     * @post All axes, statistics, and cell values are recalculated
     */
    void update(const bool player, const coordinates coo);
};

/**
 * @struct segment
 * @brief Represents a contiguous group of adjacent pieces in a specific direction.
 *
 * Tracks a run of consecutive pieces aligned in a particular direction.
 * Segments are the atomic building blocks of axes and are used for game logic,
 * win detection, and strategic evaluation.
 *
 * @details A gap (empty cell) terminates a segment; pieces on the other side
 * of the gap form a separate segment.
 */
struct segment
{
    /// @brief The direction of piece alignment in this segment
    direction d;

    /// @brief Coordinates of the first piece in this segment
    coordinates start;

    /// @brief Number of consecutive pieces in this segment (length)
    uint16_t size;

    /**
     * @brief Updates the direction of this segment.
     *
     * @param dir The new direction (H, V, D1, or D2)
     */
    void set_direction(direction dir) { d = dir; }

    segment(direction d_, coordinates start_, uint16_t size_) : d(d_), start(start_), size(size_) {};
};

/**
 * @struct Link
 * @brief Manages connections between adjacent axes in a chain.
 *
 * Links enable efficient traversal of long piece sequences by connecting
 * adjacent axes that form continuous lines. Supports dynamic linking and unlinking
 * when axes are merged or split.
 *
 * @details Uses weak pointers to avoid circular reference issues while
 * maintaining bidirectional axis relationships.
 */
struct Link
{
    /// @brief Weak pointer to the first (previous) linked axis in the chain
    std::weak_ptr<axis> axis1;

    /// @brief Weak pointer to the second (next) linked axis in the chain
    std::weak_ptr<axis> axis2;

    /**
     * @brief Assignment operator for link connections.
     *
     * @param other The Link to copy from
     * @return Reference to this Link after assignment
     */
    Link &operator=(const Link &other);

    /// @brief Default constructor initializing empty link
    Link() = default;

    /**
     * @brief Constructs a Link between two axes.
     *
     * @param a1 Weak pointer to the first (previous) axis
     * @param a2 Weak pointer to the second (next) axis
     */
    Link(std::weak_ptr<axis> a1, std::weak_ptr<axis> a2);
};

/**
 * @struct adjacent_pieces_list
 * @brief Maintains all contiguous piece segments in a specific direction.
 *
 * Caches piece groupings and gaps along a particular direction (H, V, D1, D2).
 * Provides efficient access to segment information for axis analysis and
 * game state evaluation without repeated traversal.
 *
 * @details Segments are automatically ordered by coordinate position.
 */
struct adjacent_pieces_list
{
    /// @brief Reference to the parent board structure
    const Board &board;

    /// @brief The direction of piece alignment for these segments (H, V, D1, or D2)
    direction d;

    /// @brief Vector of all contiguous piece segments in this direction
    std::vector<segment> adj;

    /**
     * @brief Retrieves the axis containing the first segment.
     *
     * Returns the axis managing this list of piece segments.
     *
     * @return Weak pointer to the axis associated with this segment list
     */
    std::weak_ptr<axis> getAxis();

    /// @brief Default constructor for empty segment list

    /**
     * @brief Constructs an adjacent_pieces_list for a specific direction.
     *
     * @param board_ Reference to the parent board
     * @param d_ The alignment direction (H, V, D1, or D2)
     */
    adjacent_pieces_list(Board &board_, direction d_) : board(board_), d(d_) {};
};

/**
 * @struct axis
 * @brief Represents a line of aligned pieces in a specific direction on the board.
 *
 * An axis tracks all pieces aligned horizontally, vertically, or diagonally.
 * Multiple axes can be linked into chains to form very long piece sequences.
 * Maintains comprehensive statistics: piece segments, gaps, longest runs,
 * and strategic value for AI evaluation and decision-making.
 *
 * @details Axes are automatically created, merged, and split by the Board
 * as pieces are placed. Inherits from enable_shared_from_this for safe self-reference.
 */
struct axis : std::enable_shared_from_this<axis>
{
    /// @brief Reference to the parent board structure
    Board &board;

    /// @brief true if this axis belongs to the player, false if opponent's axis
    bool player;

    /// @brief The direction of piece alignment (H, V, D1, or D2)
    direction d;

    /// @brief Weak pointer to the root (first/anchor) axis of the chain
    std::weak_ptr<axis> root;

    /// @brief Weak pointer to the last axis in the chain
    std::weak_ptr<axis> last;

    /// @brief Link connections to adjacent axes in the chain
    Link link;

    /// @brief Start and end coordinates defining the axis extent
    std::pair<coordinates, coordinates> extremity;

    /// @brief All piece segments and gaps in this axis
    adjacent_pieces_list adj_pcs;

    /// @brief Longest consecutive piece run: {start_coord, length}
    std::pair<coordinates, uint16_t> longest_adjacent_pieces_suite;

    /// @brief Longest potential run allowing one gap: {coord, length}
    std::pair<coordinates, uint16_t> longest_potential_adjacent_pieces_suite;

    /// @brief All gaps in the axis: vector of {gap_coord, gap_size}
    std::vector<std::pair<coordinates, uint16_t>> gaps;

    /// @brief Largest single gap in the axis: {gap_coord, gap_size}
    std::pair<coordinates, uint16_t> longest_gap;

    /// @brief Strategic value of this axis (weighted combination of statistics)
    float axis_value = 0.0;

    /**
     * @struct traverse_return
     * @brief Results of traversing all axes in a chain.
     *
     * Collects all piece segments or gaps encountered when traversing
     * through a linked axis chain.
     */
    struct traverse_return
    {
        /// @brief Direction of traversal (inherited from traversed axes)
        direction d;

        /// @brief Collection of found segments: {coordinate, size}
        std::vector<std::pair<coordinates, uint16_t>> handles;
    };

private:
    /// @brief Gets a weak pointer to this axis for enable_shared_from_this.
    std::weak_ptr<axis> self()
    {
        return shared_from_this();
    }

public:
    /**
     * @brief Gets raw pointer to this axis.
     *
     * Dereferences root pointer and performs type checking/navigation.
     *
     * @return Raw pointer to this axis
     */
    axis *get();

    /**
     * @brief Traverses the entire axis chain collecting all piece segments.
     *
     * Starting from the root, follows links through all connected axes
     * and returns all piece segments in order.
     *
     * @return traverse_return containing direction and all piece segments
     */
    traverse_return traverse_piece_axis();

    /**
     * @brief Traverses the entire axis chain collecting all gaps.
     *
     * Starting from the root, follows links through all connected axes
     * and returns all gaps in order.
     *
     * @return traverse_return containing direction and all gaps
     */
    traverse_return traverse_gaps_axis();

    /**
     * @brief Checks if a piece has a direct predecessor in this direction.
     *
     * Verifies that the previous cell in the axis direction contains a piece.
     *
     * @param coo The coordinates to check
     * @return true if previous cell contains a piece, false if empty or at boundary
     */
    bool piece_have_direct_prev(const coordinates coo);

    /**
     * @brief Checks if a piece has a direct successor in this direction.
     *
     * Verifies that the next cell in the axis direction contains a piece.
     *
     * @param coo The coordinates to check
     * @return true if next cell contains a piece, false if empty or at boundary
     */
    bool piece_have_direct_next(const coordinates coo);

private:
    /// @brief Iterator to this axis in the board's global axes list
    std::list<std::shared_ptr<axis>>::iterator self_axes;

    /// @brief Adjusts extremity coordinates to include new piece
    void modify_extremity(const coordinates coo);

    /// @brief Updates piece segment information when piece is added
    void modify_segment(const coordinates coo);

    /// @brief Updates gap information when piece is added
    void modify_gaps(const coordinates coo);

    /// @brief Recalculates all statistical metrics from current state
    void modify_stats();

    /// @brief Processes addition of a new piece to this axis
    void add_piece(const coordinates coo);

    /// @brief Synchronizes extremity across all linked axes in chain
    void sync_extremitys();

    /// @brief Updates piece references at axis endpoints
    void sync_anchors();

    /// @brief Propagates statistical metrics through linked axes
    void sync_stats();

    /// @brief Orchestrates complete axis synchronization
    void sync_axis();

    /// @brief Updates strategic values for all free cells affected by this axis
    void update_free_cells_value();

    /// @brief Orchestrates complete axis update after piece placement
    void update(const coordinates coo);

public:

    /**
     * @brief Constructs an axis with specified properties.
     *
     * @param board Reference to the parent board
     * @param player_ true if player's axis, false for opponent's axis
     * @param d_ The direction of piece alignment (H, V, D1, or D2)
     * @param ext The extremity pair (start and end coordinates)
     */
    explicit axis(Board &board, bool player_, direction d_, std::pair<coordinates, coordinates> ext);

    /// @brief Board grants access to private axis methods and members
    friend struct Board;
};

/**
 * @struct piece
 * @brief Represents a single game piece on the board.
 *
 * Each piece is identified by its player ownership, board position, and
 * the four axes (one per direction) it participates in. Pieces are the
 * fundamental game units and the basis for win detection and strategy.
 *
 * @details Uses weak pointers to axes to avoid reference cycles while
 * maintaining bidirectional piece-axis relationships.
 */
struct piece
{
    /// @brief true if piece exists and is active, false if removed
    bool exist = false;

    /// @brief true if piece belongs to the player, false if opponent's piece
    bool player = false;

    /// @brief Current board position of this piece
    coordinates coord;

    /// @brief Axes in all four directions: [H, V, D1, D2]
    /// Weak pointers prevent circular references with axis objects
    std::array<std::weak_ptr<axis>, 4> axiss;

    /// @brief Default constructor for uninitialized piece
    piece() = default;

    /**
     * @brief Constructs a piece with initial values.
     *
     * @param exist_ true if piece should be marked as existing
     * @param player_ true if piece belongs to player, false for opponent
     * @param coord_ The board position of this piece
     */
    piece(bool exist_, bool player_, coordinates coord_);
};
