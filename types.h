#pragma once
#include <iostream>
#include <vector>
#include <array>
#include <list>
#include <utility>
#include <memory>
#include <cassert>
// #include "stable_pool.hpp"

struct coordinates
{
    uint16_t x;
    uint16_t y;

    coordinates(uint16_t x = 0, uint16_t y = 0);

    bool operator<(const coordinates &other) const;
    bool operator>(const coordinates &other) const;
    bool operator==(const coordinates &other) const;
    bool operator>=(const coordinates &other) const { return !(*this < other); }
    bool operator<=(const coordinates &other) const { return !(*this > other); }
    bool operator!=(const coordinates &other) const { return !(*this == other); }
};

enum class direction
{
    H = 0,
    V = 1,
    D1 = 2,
    D2 = 3
};

struct piece;

struct cell
{
    std::shared_ptr<piece> pc;

    bool free() const;

    float ai_offensive_value = 0;
    float ai_defensive_value = 0;
    float ai_general_value() const;

    float opponent_offensive_value = 0;
    float opponent_defensive_value = 0;
    float opponent_general_value() const;

    float game_offensive_value() const;
    float game_defensive_value() const;
    float game_general_value() const;

    float beam_search_bonus = 0;
    float MLP_beam_search_bonus = 0;
    float MLP_final_decision_bonus = 0;

    cell() = default;
    cell(std::unique_ptr<piece> pc_);
};

struct axis;

struct Board
{
    std::vector<cell> board;
    uint16_t size;
    uint16_t number_piece_to_align_to_win;

    Board(uint16_t s, uint16_t k) : board(s * s), size(uint16_t(s)), number_piece_to_align_to_win(k) {}

    std::list<std::shared_ptr<axis>> axes;

    bool inside(coordinates coo) const
    {
        return coo.x < size && coo.y < size;
    }

    cell &operator[](int i)
    {
        return board[i];
    }

    cell &at1D(int i)
    {
        return board.at(i);
    }

    const cell &at1D(int i) const
    {
        return board.at(i);
    }

    cell &at2D(int x, int y)
    {
        assert(y < size);
        assert(x < size);
        return board.at(y * size + x);
    }

    const cell &at2D(int x, int y) const
    {
        assert(y < size);
        assert(x < size);
        return board.at(y * size + x);
    }

    cell &at2D(coordinates coo)
    {
        return at2D(coo.x, coo.y);
    }

    const cell &at2D(coordinates coo) const
    {
        return at2D(coo.x, coo.y);
    }

    uint16_t distance(coordinates cooA, coordinates cooB)
    {
        return std::max(std::max(cooA.x, cooB.x) - std::min(cooA.x, cooB.x), std::max(cooA.y, cooB.y) - std::min(cooA.y, cooB.y));
    }

    direction get_direction(const coordinates A, const coordinates B);

    coordinates prev(const direction d, coordinates coo);
    coordinates next(const direction d, coordinates coo);
    coordinates jump(const direction d, const bool way, const coordinates start, uint16_t distance);

    void create_axis(const bool player, const coordinates A, const coordinates B);
    void remove_axis(std::shared_ptr<axis> axe);
    void merge_axis(std::weak_ptr<axis> wcurrent, std::weak_ptr<axis> wother);
    void split_axis(std::weak_ptr<axis> waxis, const coordinates coo);
    void put_piece(const bool player, const coordinates coo);
    void update(const bool player, const coordinates coo);
};

struct segment
{
    direction d;
    coordinates start;
    uint16_t size;
    void set_direction(direction dir) { d = dir; }
};

struct Link
{
    std::weak_ptr<axis> axis1;
    std::weak_ptr<axis> axis2;

    Link &operator=(const Link &other);

    Link() = default;
    Link(std::weak_ptr<axis> a1, std::weak_ptr<axis> a2);
};

struct adjacent_pieces_list
{
    const Board &board;
    direction d;
    std::vector<segment> adj;
    // void set_direction(direction dir) { d = dir; }
    std::weak_ptr<axis> getAxis();

    adjacent_pieces_list() = default;
    adjacent_pieces_list(Board &board_, direction d_) : board(board_), d(d_) {};
};

struct axis : std::enable_shared_from_this<axis>
{
    Board &board;
    bool player;
    direction d;
    std::weak_ptr<axis> root;
    std::weak_ptr<axis> last;
    Link link;
    std::pair<coordinates, coordinates> extremity;
    adjacent_pieces_list adj_pcs;
    std::pair<coordinates, uint16_t> longest_adjacent_pieces_suite;
    std::pair<coordinates, uint16_t> longest_potential_adjacent_pieces_suite;
    std::vector<std::pair<coordinates, uint16_t>> gaps;
    std::pair<coordinates, uint16_t> longest_gap;

    struct traverse_return
    {
        direction d;
        std::vector<std::pair<coordinates, uint16_t>> handles;
    };

private:
    std::weak_ptr<axis> self()
    {
        return shared_from_this();
    }

public:
    axis *get();
    traverse_return traverse_piece_axis();
    traverse_return traverse_gaps_axis();
    bool piece_have_direct_prev(const coordinates coo);
    bool piece_have_direct_next(const coordinates coo);

private:
    std::list<std::shared_ptr<axis>>::iterator self_axes;

    void modify_extremity(const coordinates coo); // update axe local
    void modify_segment(const coordinates coo);   // update axe local
    void modify_gaps(const coordinates coo);      // update axe local
    void modify_stats();                          // update axe local
    void add_piece(const coordinates coo);        // update axe local

    void sync_extremitys();             // update axe logique
    void sync_anchors();                // update axe logique
    void sync_stats();                  // update axe logique
    void sync_axis();                   // update axe logique
    void update(const coordinates coo); // update axe logique

public:
    axis() = default;
    explicit axis(Board &board, bool player_, direction d_, std::pair<coordinates, coordinates> ext);
    friend struct Board;
};

struct piece
{
    bool exist = false;
    bool player = false;
    coordinates coord;
    std::array<std::weak_ptr<axis>, 4> axiss;

    piece() = default;
    piece(bool exist_, bool player_, coordinates coord_);
};