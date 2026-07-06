#pragma once
// types.h - structures de base pour le plateau et l'IA

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

// coordonnées d'une case sur le plateau
struct coordinates
{
    uint16_t x;
    uint16_t y;

    coordinates(uint16_t x = 0, uint16_t y = 0);

    bool operator<(const coordinates &other) const;
    bool operator>(const coordinates &other) const;
    bool operator==(const coordinates &other) const;
    bool operator!=(const coordinates &other) const;
    bool operator<=(const coordinates &other) const;
    bool operator>=(const coordinates &other) const;
};

// orientation pour l'alignement des pièces
enum class direction
{
    H = 0,
    V = 1,
    D1 = 2,
    D2 = 3
};

struct piece;

// case du plateau avec valeurs heuristiques
struct cell
{
    std::shared_ptr<piece> pc; // pièce sur la case, nullptr si aucune
    bool free() const;
    float offensive_value = 0.0;                                        // attirance pour l'attaque
    float defensive_value = 0.0;                                        // importance pour la défense
    std::array<std::pair<bool, float>, 4> axis_values{{{false, 0.0f}}}; // valeur par direction
    float general_value() const;
    void update_values(const direction d, const bool player, const float value);
    cell() = default;
    cell(std::unique_ptr<piece> pc_);
};

struct axis;

// plateau et axes de pièces alignées
struct Board
{
    std::vector<cell> board;               // stockage linéaire du plateau
    uint16_t size;                         // taille du plateau (carré)
    uint16_t number_piece_to_align_to_win; // nombre de pièces nécessaires pour gagner
    std::list<std::shared_ptr<axis>> axes; // axes actifs pour l'évaluation

    Board(uint16_t s, uint16_t k) : board(s * s), size(s), number_piece_to_align_to_win(k) {}

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

    // calculs de voisinage et de direction pour alignements
    direction get_direction(const coordinates A, const coordinates B);
    std::optional<coordinates> prev(const direction d, coordinates coo);
    std::optional<coordinates> next(const direction d, coordinates coo);
    std::optional<coordinates> jump(const direction d, const bool way, const coordinates start, uint16_t distance);
    void create_axis(const bool player, const coordinates A, const coordinates B);
    void remove_axis(std::shared_ptr<axis> axe);
    void merge_axis(std::weak_ptr<axis> wcurrent, std::weak_ptr<axis> wother);
    void split_axis(std::weak_ptr<axis> waxis, const coordinates coo);
    void put_piece(const bool player, const coordinates coo);
    void update_cells_values();
    void update(const bool player, const coordinates coo);
};

struct segment
{
    direction d;
    coordinates start;
    uint16_t size;
    void set_direction(direction dir) { d = dir; }
    segment(direction d_, coordinates start_, uint16_t size_) : d(d_), start(start_), size(size_) {};
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
    std::weak_ptr<axis> getAxis();
    adjacent_pieces_list(Board &board_, direction d_) : board(board_), d(d_) {};
};

// axe = ligne de pièces alignées et ses données d'évaluation
struct axis : std::enable_shared_from_this<axis>
{
    Board &board;                                                             // plateau lié à cet axe
    bool player;                                                              // joueur qui possède ces pièces
    direction d;                                                              // orientation de l'axe
    std::weak_ptr<axis> root;                                                 // axe racine après fusion
    std::weak_ptr<axis> last;                                                 // dernier axe de la chaîne
    Link link;                                                                // lien avec un autre axe
    std::pair<coordinates, coordinates> extremity;                            // bornes de l'axe
    adjacent_pieces_list adj_pcs;                                             // pièces adjacentes au segment
    std::pair<coordinates, uint16_t> longest_adjacent_pieces_suite;           // suite contiguë la plus longue
    std::pair<coordinates, uint16_t> longest_potential_adjacent_pieces_suite; // suite potentielle avec espaces
    std::vector<std::pair<coordinates, uint16_t>> gaps;                       // trous dans l'axe
    std::pair<coordinates, uint16_t> longest_gap;                             // trou le plus long
    float axis_value = 0.0;                                                   // valeur heuristique globale de l'axe

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
    void modify_extremity(const coordinates coo);
    void modify_segment(const coordinates coo);
    void modify_gaps(const coordinates coo);
    void modify_stats();
    void add_piece(const coordinates coo);
    void sync_extremitys();
    void sync_anchors();
    void sync_stats();
    void sync_axis();
    void update_free_cells_value();
    void update(const coordinates coo);

public:
    explicit axis(Board &board, bool player_, direction d_, std::pair<coordinates, coordinates> ext);
    friend struct Board;
};
