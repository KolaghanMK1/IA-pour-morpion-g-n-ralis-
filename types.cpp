#include "types.h"

// coordinates
coordinates::coordinates(uint16_t x_, uint16_t y_) : x(x_), y(y_) {}

bool coordinates::operator<(const coordinates &other) const
{
    if (x != other.x)
        return x < other.x;
    return y < other.y;
}

bool coordinates::operator>(const coordinates &other) const
{
    if (x != other.x)
        return x > other.x;
    return y > other.y;
}

bool coordinates::operator==(const coordinates &other) const
{
    return x == other.x && y == other.y;
}

// cell
cell::cell(std::unique_ptr<piece> pc_) : pc(std::move(pc_)) {}

bool cell::free() const
{
    return !pc || !pc->exist;
}

float cell::ai_general_value() const
{
    return (ai_offensive_value + ai_defensive_value) / 2;
}

float cell::opponent_general_value() const
{
    return (opponent_offensive_value + opponent_defensive_value) / 2;
}

float cell::game_offensive_value() const
{
    return (ai_offensive_value + opponent_offensive_value) / 2;
}

float cell::game_defensive_value() const
{
    return (ai_defensive_value + opponent_defensive_value) / 2;
}

float cell::game_general_value() const
{
    return (ai_general_value() + opponent_general_value()) / 2;
}

// Board
direction Board::get_direction(const coordinates A, const coordinates B)
{
    assert(inside(A) && inside(B));
    int dx = A.x - B.x;
    int dy = A.y - B.y;
    direction d;
    if (dx != 0 && dy == 0)
        d = direction::H;
    else if (dx == 0 && dy != 0)
        d = direction::V;
    else if (abs(dx) == abs(dy))
    {
        if (dx * dy > 0)
            d = direction::D1;
        else
            d = direction::D2;
    }
    else
    {
        std::cerr << "Direction invalide";
        std::abort();
    }
    return d;
}

coordinates Board::prev(const direction d, coordinates coo)
{
    switch (d)
    {
    case direction::H:
        if (inside(coo) && coo.x != 0 && inside({static_cast<uint16_t>(coo.x - 1), coo.y}))
            return {static_cast<uint16_t>(coo.x - 1), coo.y};
        else
        {
            std::cerr << "Board::prev -- Invalid coordinates";
            return coo;
        }
    case direction::V:
        if (inside(coo) && coo.y != 0 && inside({coo.x, static_cast<uint16_t>(coo.y - 1)}))
            return {coo.x, static_cast<uint16_t>(coo.y - 1)};
        else
        {
            std::cerr << "Board::prev -- Invalid coordinates";
            return coo;
        }
    case direction::D1:
        if (inside(coo) && coo.x != 0 && coo.y != 0 && inside({static_cast<uint16_t>(coo.x - 1), static_cast<uint16_t>(coo.y - 1)}))
            return {static_cast<uint16_t>(coo.x - 1), static_cast<uint16_t>(coo.y - 1)};
        else
        {
            std::cerr << "Board::prev -- Invalid coordinates";
            return coo;
        }
    case direction::D2:
        if (inside(coo) && coo.x != 0 && inside({static_cast<uint16_t>(coo.x - 1), static_cast<uint16_t>(coo.y + 1)}))
            return {static_cast<uint16_t>(coo.x - 1), static_cast<uint16_t>(coo.y + 1)};
        else
        {
            std::cerr << "Board::prev -- Invalid coordinates";
            return coo;
        }
    default:
        std::unreachable(); // A absolument activer pour compilation + préciser -std=c++23 dans commande
        // assert(false);
        // return {0,0};
    }
}

coordinates Board::next(const direction d, coordinates coo)
{
    switch (d)
    {
    case direction::H:
        if (inside(coo) && inside({static_cast<uint16_t>(coo.x + 1), coo.y}))
            return {static_cast<uint16_t>(coo.x + 1), coo.y};
        else
        {
            std::cerr << "Board::next -- Invalid coordinates";
            return coo;
        }
    case direction::V:
        if (inside(coo) && inside({coo.x, static_cast<uint16_t>(coo.y + 1)}))
            return {coo.x, static_cast<uint16_t>(coo.y + 1)};
        else
        {
            std::cerr << "Board::next -- Invalid coordinates";
            return coo;
        }
    case direction::D1:
        if (inside(coo) && inside({static_cast<uint16_t>(coo.x + 1), static_cast<uint16_t>(coo.y + 1)}))
            return {static_cast<uint16_t>(coo.x + 1), static_cast<uint16_t>(coo.y + 1)};
        else
        {
            std::cerr << "Board::next -- Invalid coordinates";
            return coo;
        }
    case direction::D2:
        if (inside(coo) && coo.y != 0 && inside({static_cast<uint16_t>(coo.x + 1), static_cast<uint16_t>(coo.y - 1)}))
            return {static_cast<uint16_t>(coo.x + 1), static_cast<uint16_t>(coo.y - 1)};
        else
        {
            std::cerr << "Board::next -- Invalid coordinates";
            return coo;
        }
    default:
        std::unreachable(); // A absolument activer pour compilation + préciser -std=c++23 dans commande
        // assert(false && "Invalid axis direction");
        // return coo;
    }
}

coordinates Board::jump(const direction d, const bool way, const coordinates coo, uint16_t distance)
{
    int nx;
    int ny;
    switch (d)
    {
    case direction::H:
        if (way)
            nx = coo.x + distance;
        else
            nx = coo.x - distance;
        if (nx >= 0 && inside(coo) && inside({static_cast<uint16_t>(nx), coo.y}))
            return {static_cast<uint16_t>(nx), coo.y};
        else
        {
            std::cerr << "Board::next -- Invalid coordinates";
            return coo;
        }
    case direction::V:
        if (way)
            ny = coo.y + distance;
        else
            ny = coo.y - distance;
        if (ny >= 0 && inside(coo) && inside({coo.x, static_cast<uint16_t>(ny)}))
            return {coo.x, static_cast<uint16_t>(ny)};
        else
        {
            std::cerr << "Board::next -- Invalid coordinates";
            return coo;
        }
    case direction::D1:
        if (way)
        {
            nx = coo.x + distance;
            ny = coo.y + distance;
        }
        else
        {
            nx = coo.x - distance;
            ny = coo.y - distance;
        }
        if (nx >= 0 && ny >= 0 && inside(coo) && inside({static_cast<uint16_t>(nx), static_cast<uint16_t>(ny)}))
            return {static_cast<uint16_t>(nx), static_cast<uint16_t>(ny)};
        else
        {
            std::cerr << "Board::next -- Invalid coordinates";
            return coo;
        }
    case direction::D2:
        if (way)
        {
            nx = coo.x + distance;
            ny = coo.y - distance;
        }
        else
        {
            nx = coo.x - distance;
            ny = coo.y + distance;
        }
        if (nx >= 0 && ny >= 0 && inside(coo) && coo.y != 0 && inside({static_cast<uint16_t>(nx), static_cast<uint16_t>(ny)}))
            return {static_cast<uint16_t>(nx), static_cast<uint16_t>(ny)};
        else
        {
            std::cerr << "Board::next -- Invalid coordinates";
            return coo;
        }
    default:
        std::unreachable(); // A absolument activer pour compilation + préciser -std=c++23 dans commande
        // assert(false && "Invalid axis direction");
        // return coo;
    }
}

void Board::create_axis(const bool player, const coordinates A, const coordinates B)
{
    direction d = get_direction(A, B);

    axes.push_back(std::make_shared<axis>(*this, player, d, std::make_pair(A, B)));
    std::list<std::shared_ptr<axis>>::iterator temp_it = std::prev(axes.end());
    axis *axe = axes.back().get();
    axe->self_axes = temp_it;
    axe->root = axe->last = axe->self();
    at2D(A).pc.get()->axiss.at(static_cast<int>(d)) = axe->self();
    at2D(B).pc.get()->axiss.at(static_cast<int>(d)) = axe->self();
}

void Board::remove_axis(std::shared_ptr<axis> axe)
{
    axes.erase(axe->self_axes);
}

void Board::merge_axis(std::weak_ptr<axis> wcurrent, std::weak_ptr<axis> wother)
{
    axis *current, *other;
    std::shared_ptr<axis> scurrent, sother;
    if ((scurrent = wcurrent.lock()) && (sother = wother.lock()))
    {
        current = scurrent.get();
        other = sother.get();
        if (current->player != other->player || current->d != other->d)
            return;
        if (current->extremity < other->extremity)
        {
            current->extremity.second = other->extremity.second;
            other->extremity.first = current->extremity.first;
            current->link.axis2 = other->self();
            other->link.axis1 = current->self();
            if (other->last.lock())
            {
                current->last = other->last;
            }
            if (current->root.lock())
            {
                other->root = current->root;
            }
        }
        else
        {
            current->extremity.first = other->extremity.first;
            other->extremity.second = current->extremity.second;
            current->link.axis1 = other->self();
            other->link.axis2 = current->self();
            if (current->last.lock())
            {
                other->last = current->last;
            }
            if (other->root.lock())
            {
                current->root = other->root;
            }
        }
    }
}

void Board::split_axis(std::weak_ptr<axis> wptr_axis, const coordinates coo)
{
    std::weak_ptr<axis> wptr_other_axis;
    std::shared_ptr<axis> sptr_axis;
    std::shared_ptr<axis> sptr_other_axis;
    axis *current_axis;
    axis *other_axis;
    if ((sptr_axis = wptr_axis.lock()))
    {
        current_axis = sptr_axis.get();
        if (coo < current_axis->adj_pcs.adj.front().start)
        {
            wptr_other_axis = current_axis->link.axis1;
            if ((sptr_other_axis = wptr_other_axis.lock()))
            {
                other_axis = sptr_other_axis.get();

                current_axis->link.axis1.reset();
                other_axis->link.axis2.reset();

                current_axis->extremity.first = current_axis->adj_pcs.adj.front().start;
                other_axis->extremity.second = jump(other_axis->d, true, other_axis->adj_pcs.adj.back().start, other_axis->adj_pcs.adj.back().size);

                current_axis->sync_axis();
                other_axis->sync_axis();
            }
        }
        else if (coo > jump(current_axis->d, true, current_axis->adj_pcs.adj.back().start, current_axis->adj_pcs.adj.back().size))
        {
            wptr_other_axis = current_axis->link.axis2;
            if ((sptr_other_axis = wptr_other_axis.lock()))
            {
                other_axis = sptr_other_axis.get();

                current_axis->link.axis2.reset();
                other_axis->link.axis1.reset();

                current_axis->extremity.second = jump(current_axis->d, true, current_axis->adj_pcs.adj.back().start, current_axis->adj_pcs.adj.back().size);
                other_axis->extremity.first = other_axis->adj_pcs.adj.front().start;

                current_axis->sync_axis();
                other_axis->sync_axis();
            }
        }
        else
        {
            axis *new_axis;
            adjacent_pieces_list segments_new_axis(current_axis->board, current_axis->d);
            std::vector<std::pair<coordinates, uint16_t>> gaps_new_axis;

            std::vector<std::pair<coordinates, uint16_t>>::iterator splited_gap;
            std::vector<segment>::iterator last_segment;

            bool found_gap = false, found_segment = false;
            for (std::vector<std::pair<coordinates, uint16_t>>::iterator it = current_axis->gaps.begin(); it != current_axis->gaps.end(); ++it)
            {
                if (coo >= it->first && coo <= jump(current_axis->d, true, it->first, it->second))
                {
                    splited_gap = it;
                    found_gap = true;
                    break;
                }
            }
            for (std::vector<segment>::iterator it = current_axis->adj_pcs.adj.begin(); std::next(it) != current_axis->adj_pcs.adj.end(); ++it)
            {
                if (coo > it->start && coo < std::next(it)->start)
                {
                    last_segment = it;
                    found_segment = true;
                    break;
                }
            }

            assert(found_gap && found_segment);

            std::move(std::next(splited_gap), current_axis->gaps.end(), std::back_inserter(gaps_new_axis));
            std::move(std::next(last_segment), current_axis->adj_pcs.adj.end(), std::back_inserter(segments_new_axis.adj));
            current_axis->gaps.erase(splited_gap);
            create_axis(current_axis->player, segments_new_axis.adj.front().start, jump(current_axis->d, true, segments_new_axis.adj.back().start, segments_new_axis.adj.back().size));
            new_axis = axes.back().get();
            new_axis->adj_pcs.d = segments_new_axis.d;
            new_axis->adj_pcs.adj = segments_new_axis.adj;
            new_axis->gaps = gaps_new_axis;

            new_axis->link.axis2 = current_axis->link.axis2;
            if ((sptr_other_axis = current_axis->link.axis2.lock()))
            {
                other_axis = sptr_other_axis.get();
                other_axis->link.axis1 = new_axis->self();
            }

            current_axis->link.axis2.reset();
            current_axis->modify_stats();
            current_axis->sync_axis();

            new_axis->modify_stats();
            new_axis->sync_axis();
        }
    }
}

void Board::put_piece(const bool player, const coordinates coo)
{
    assert(inside(coo) && at2D(coo).free());
    at2D(coo).pc = std::make_shared<piece>(true, player, coo);
}

void Board::update(const bool player, const coordinates coo)
{
    assert(inside(coo));
    uint16_t max_gap_size;
    if (number_piece_to_align_to_win % 2 == 0)
        max_gap_size = number_piece_to_align_to_win / 2;
    else
        max_gap_size = number_piece_to_align_to_win / 2 + 1;

    std::vector<coordinates> around(8, coo);
    std::vector<bool> find(8, false);
    std::vector<bool> blocked(8, false);
    bool all_find = true;
    for (uint16_t i = 0; i < max_gap_size; i++)
    {
        all_find = true;
        // H -
        if (!blocked.at(0) && !find.at(0))
        {
            coordinates p = prev(direction::H, around.at(0));
            if (p != around.at(0))
            {
                around.at(0) = p;
                if (!at2D(p).free())
                {
                    find.at(0) = at2D(p).pc->player == player;
                    blocked.at(0) = !find.at(0);
                }
            }
            else
                all_find = false;
        }

        // V -
        if (!blocked.at(1) && !find.at(1))
        {
            coordinates p = prev(direction::V, around.at(1));
            if (p != around.at(1))
            {
                around.at(1) = p;
                if (!at2D(p).free())
                {
                    find.at(1) = at2D(p).pc->player == player;
                    blocked.at(1) = !find.at(1);
                }
            }
            else
                all_find = false;
        }

        // D1 -
        if (!blocked.at(2) && !find.at(2))
        {
            coordinates p = prev(direction::D1, around.at(2));
            if (p != around.at(2))
            {
                around.at(2) = p;
                if (!at2D(p).free())
                {
                    find.at(2) = at2D(p).pc->player == player;
                    blocked.at(2) = !find.at(2);
                }
            }
            else
                all_find = false;
        }

        // D2 -
        if (!blocked.at(3) && !find.at(3))
        {
            coordinates p = prev(direction::D2, around.at(3));
            if (p != around.at(3))
            {
                around.at(3) = p;
                if (!at2D(p).free())
                {
                    find.at(3) = at2D(p).pc->player == player;
                    blocked.at(3) = !find.at(3);
                }
            }
            else
                all_find = false;
        }

        // H +
        if (!blocked.at(4) && !find.at(4))
        {
            coordinates p = next(direction::H, around.at(4));
            if (p != around.at(4))
            {
                around.at(4) = p;
                if (!at2D(p).free())
                {
                    find.at(4) = at2D(p).pc->player == player;
                    blocked.at(4) = !find.at(4);
                }
            }
            else
                all_find = false;
        }

        // V +
        if (!blocked.at(5) && !find.at(5))
        {
            coordinates p = next(direction::V, around.at(5));
            if (p != around.at(5))
            {
                around.at(5) = p;
                if (!at2D(p).free())
                {
                    find.at(5) = at2D(p).pc->player == player;
                    blocked.at(5) = !find.at(5);
                }
            }
            else
                all_find = false;
        }

        // D1 +
        if (!blocked.at(6) && !find.at(6))
        {
            coordinates p = next(direction::D1, around.at(6));
            if (p != around.at(6))
            {
                around.at(6) = p;
                if (!at2D(p).free())
                {
                    find.at(6) = at2D(p).pc->player == player;
                    blocked.at(6) = !find.at(6);
                }
            }
            else
                all_find = false;
        }

        // D2 +
        if (!blocked.at(7) && !find.at(7))
        {
            coordinates p = next(direction::D2, around.at(7));
            if (p != around.at(7))
            {
                around.at(7) = p;
                if (!at2D(p).free())
                {
                    find.at(7) = at2D(p).pc->player == player;
                    blocked.at(7) = !find.at(7);
                }
            }
            else
                all_find = false;
        }
        if (all_find)
            break;
    }

    std::weak_ptr<axis> wptr_axis;
    std::shared_ptr<axis> sptr_axis;
    for (int i = 0; i < 8; i++)
    {
        if (find.at(i) && !blocked.at(i))
        {
            direction d = get_direction(coo, around.at(i));
            std::weak_ptr<axis> wptr_axis = at2D(around.at(i)).pc->axiss.at(static_cast<int>(d));
            if (wptr_axis.expired())
            {
                create_axis(player, coo, around.at(i));
                wptr_axis = at2D(coo).pc.get()->axiss.at(static_cast<int>(d));
                if ((sptr_axis = wptr_axis.lock()))
                    sptr_axis.get()->update(coo);
            }
        }
    }
    for (int i = 0; i < 4; i++)
    {
        if ((find.at(i) && !blocked.at(i)) && (find.at(i + 4) && !blocked.at(i + 4)))
        {
            direction d = get_direction(coo, around.at(i));
            std::weak_ptr<axis> wptr_axis1 = at2D(around.at(i)).pc->axiss.at(static_cast<int>(d));
            std::shared_ptr<axis> sptr_axis1;
            std::weak_ptr<axis> wptr_axis2 = at2D(around.at(i + 4)).pc->axiss.at(static_cast<int>(d));
            std::shared_ptr<axis> sptr_axis2;
            if ((sptr_axis1 = wptr_axis1.lock()) && (sptr_axis2 = wptr_axis2.lock()))
            {
                merge_axis(wptr_axis1, wptr_axis2);
                sptr_axis1.get()->update(coo);
            }
        }
    }
}

// adjacent_pieces_list
std::weak_ptr<axis> adjacent_pieces_list::getAxis()
{
    return board.at2D(adj.front().start).pc.get()->axiss.at(static_cast<int>(d));
}

// axis
axis *axis::get()
{
    if (std::shared_ptr<axis> sp_axis = self().lock())
        return sp_axis.get();
    else
        return nullptr;
}

axis::traverse_return axis::traverse_piece_axis()
{
    axis *axe = nullptr;
    std::shared_ptr<axis> sptr_axis;
    traverse_return result;

    if ((sptr_axis = root.lock()))
        axe = sptr_axis.get();
    result.d = this->d;

    while (axe != nullptr)
    {
        for (segment seg : axe->adj_pcs.adj)
        {
            result.handles.emplace_back(seg.start, int(seg.size));
        }
        if ((sptr_axis = axe->link.axis2.lock()))
            axe = sptr_axis.get();
        else
            break;
    }

    return result;
}

axis::traverse_return axis::traverse_gaps_axis()
{
    axis *axe = nullptr;
    std::shared_ptr<axis> sptr_axis;
    traverse_return result;

    if ((sptr_axis = root.lock()))
        axe = sptr_axis.get();
    result.d = this->d;

    while (axe != nullptr)
    {
        for (std::pair<coordinates, int> gap : axe->gaps)
        {
            result.handles.emplace_back(gap);
        }
        if ((sptr_axis = axe->link.axis2.lock()))
            axe = sptr_axis.get();
        else
            break;
    }

    return result;
}

bool axis::piece_have_direct_prev(const coordinates coo)
{
    assert(board.prev(d, coo) != coo); // Pour debug uniquement
    return !board.at2D(board.prev(d, coo)).free();
}

bool axis::piece_have_direct_next(const coordinates coo)
{
    assert(board.next(d, coo) != coo); // Pour debug uniquement
    return !board.at2D(board.next(d, coo)).free();
}

// - root holds the authoritative first extremity
// - last holds the authoritative second extremity
// - intermediate axes must not be queried for extremities

void axis::modify_extremity(const coordinates coo)
{
    if (coo < extremity.first)
        extremity.first = coo;
    else if (coo > extremity.second)
        extremity.second = coo;
}

void axis::modify_segment(const coordinates coo)
{
    const bool has_prev = piece_have_direct_prev(coo);
    const bool has_next = piece_have_direct_next(coo);

    if (has_prev && !has_next)
    {
        for (std::vector<segment>::iterator it = adj_pcs.adj.begin(); it != adj_pcs.adj.end(); ++it)
        {
            if (it->start > coo && it != adj_pcs.adj.begin())
            {
                --it;
                it->size++;
                return;
            }
        }
        adj_pcs.adj.back().size++;
        return;
    }
    else if (!has_prev && has_next)
    {
        for (std::vector<segment>::iterator it = adj_pcs.adj.begin(); it != adj_pcs.adj.end(); ++it)
        {
            if (it->start > coo)
            {
                it->start = coo;
                it->size++;
                return;
            }
        }
    }
    else if (has_prev && has_next)
    {
        std::vector<segment>::iterator next;
        for (std::vector<segment>::iterator it = adj_pcs.adj.begin(); std::next(it) != adj_pcs.adj.end(); ++it)
        {
            next = std::next(it);
            if (next->start > coo)
            {
                it->size = it->size + next->size + 1;
                adj_pcs.adj.erase(next);
                return;
            }
        }
    }
    else
    {
        std::vector<segment>::iterator next;
        for (std::vector<segment>::iterator it = adj_pcs.adj.begin(); std::next(it) != adj_pcs.adj.end(); ++it)
        {
            next = std::next(it);
            if (next->start > coo)
            {
                adj_pcs.adj.insert(std::next(it), {d, coo, 1});
                return;
            }
        }
        adj_pcs.adj.emplace_back(d, coo, 1);
        return;
    }
}

void axis::modify_gaps(const coordinates coo)
{
    const bool has_prev = piece_have_direct_prev(coo);
    const bool has_next = piece_have_direct_next(coo);

    if (gaps.empty())
        return;
    else if (has_prev && !has_next)
    {
        for (std::vector<std::pair<coordinates, uint16_t>>::iterator it = gaps.begin(); it != gaps.end(); ++it)
        {
            if (it->first == coo)
            {
                it->first = board.next(d, it->first);
                it->second--;
                return;
            }
        }
    }
    else if (!has_prev && has_next)
    {
        for (std::vector<std::pair<coordinates, uint16_t>>::iterator it = gaps.begin(); it != gaps.end(); ++it)
        {
            if (it->first < coo && it != gaps.end() && std::next(it)->first > coo)
            {
                it->second--;
                return;
            }
        }
        gaps.back().second--;
        return;
    }
    else if (has_prev && has_next)
    {
        for (std::vector<std::pair<coordinates, uint16_t>>::iterator it = gaps.begin(); it != gaps.end(); ++it)
        {
            if (it->first == coo)
            {
                gaps.erase(it);
                return;
            }
        }
    }
    else // Zone à risque
    {
        size_t distance;
        size_t temp;
        for (std::vector<std::pair<coordinates, uint16_t>>::iterator it = gaps.begin(); std::next(it) != gaps.end(); ++it)
        {
            if (it->first < coo)
            {
                it = std::next(it);
                distance = board.distance(coo, it->first) - 1;
                temp = it->second;
                it->second = distance;
                gaps.insert(std::next(it), {board.next(d, coo), temp - distance - 1});
                return;
            }
        }
        if (coo < gaps.front().first)
        {
            gaps.insert(gaps.begin(), {board.next(d, coo), board.distance(coo, gaps.front().first)});
            return;
        }
        else
        {
            gaps.emplace_back(coo, board.distance(coo, gaps.back().first));
            return;
        }
    }
}

void axis::modify_stats()
{
    longest_adjacent_pieces_suite.second = 0;
    longest_potential_adjacent_pieces_suite.second = 0;
    longest_gap.second = 0;

    for (std::vector<segment>::iterator it = adj_pcs.adj.begin(); it != adj_pcs.adj.end(); ++it)
    {
        if (it->size > longest_adjacent_pieces_suite.second)
        {
            longest_adjacent_pieces_suite.first = it->start;
            longest_adjacent_pieces_suite.second = it->size;
        }
    }

    for (std::vector<std::pair<coordinates, uint16_t>>::iterator it_gap = gaps.begin(); std::next(it_gap) != gaps.end(); ++it_gap)
    {
        if (it_gap->second > longest_gap.second)
        {
            longest_gap = *it_gap;
        }

        if (it_gap->second == 1)
        {
            for (std::vector<segment>::iterator it_seg = adj_pcs.adj.begin(); it_seg != std::prev(adj_pcs.adj.end()); ++it_seg)
            {
                if (it_seg->start < it_gap->first && std::next(it_seg)->start > it_gap->first)
                {
                    if (longest_potential_adjacent_pieces_suite.second < it_seg->size + std::next(it_seg)->size + 1)
                    {
                        longest_potential_adjacent_pieces_suite.first = it_seg->start;
                        longest_potential_adjacent_pieces_suite.second = it_seg->size + std::next(it_seg)->size + 1;
                    }
                }
            }
        }
    }
}

void axis::add_piece(const coordinates coo)
{
    modify_extremity(coo);
    modify_segment(coo);
    modify_gaps(coo);
    modify_stats();
}

void axis::sync_extremitys()
{
    std::shared_ptr<axis> other_axis;
    std::shared_ptr<axis> sptr_current;
    axis *current;
    if ((sptr_current = self().lock()))
    {
        if (root.lock() == sptr_current)
        {
            current = sptr_current.get();
            while ((other_axis = current->link.axis2.lock()))
            {
                other_axis.get()->extremity.first = extremity.first;
                sptr_current = other_axis;
                current = sptr_current.get();
            }
        }
        else if (last.lock() == sptr_current)
        {
            current = sptr_current.get();
            while ((other_axis = current->link.axis1.lock()))
            {
                other_axis.get()->extremity.second = extremity.second;
                sptr_current = other_axis;
                current = sptr_current.get();
            }
        }
    }
}

void axis::sync_anchors()
{
    std::weak_ptr<axis> wptr_anchor;
    std::shared_ptr<axis> sptr_anchor;
    std::shared_ptr<axis> sptr_current;
    axis *anchor;
    axis *current;
    piece *pcs = board.at2D(extremity.first).pc.get();

    wptr_anchor = pcs->axiss.at(static_cast<int>(d));
    if ((sptr_anchor = wptr_anchor.lock()))
    {
        current = anchor = sptr_anchor.get();
        anchor->root = sptr_anchor;
        while ((sptr_current = current->link.axis2.lock()))
        {
            sptr_current.get()->root = wptr_anchor;
            current = sptr_current.get();
        }
    }

    pcs = board.at2D(extremity.second).pc.get();
    wptr_anchor = pcs->axiss.at(static_cast<int>(d));
    if ((sptr_anchor = wptr_anchor.lock()))
    {
        current = anchor = sptr_anchor.get();
        anchor->root = wptr_anchor;
        while ((sptr_current = current->link.axis1.lock()))
        {
            sptr_current.get()->last = wptr_anchor;
            current = sptr_current.get();
        }
    }
}

void axis::sync_stats()
{
    std::shared_ptr<axis> sptr_current_axis;
    axis *current_axis;
    std::pair<coordinates, uint16_t> absolute_longest_adjacent_pieces_suite;
    std::pair<coordinates, uint16_t> absolute_longest_potential_adjacent_pieces_suite;
    std::pair<coordinates, uint16_t> absolute_longest_gap;
    if ((sptr_current_axis = root.lock()))
    {
        current_axis = sptr_current_axis.get();
        absolute_longest_adjacent_pieces_suite = current_axis->longest_adjacent_pieces_suite;
        absolute_longest_potential_adjacent_pieces_suite = current_axis->longest_potential_adjacent_pieces_suite;
        absolute_longest_gap = current_axis->longest_gap;

        while (current_axis != nullptr)
        {
            if (absolute_longest_adjacent_pieces_suite.second < current_axis->longest_adjacent_pieces_suite.second)
                absolute_longest_adjacent_pieces_suite = current_axis->longest_adjacent_pieces_suite;

            if (absolute_longest_potential_adjacent_pieces_suite.second < current_axis->longest_potential_adjacent_pieces_suite.second)
                absolute_longest_potential_adjacent_pieces_suite = current_axis->longest_potential_adjacent_pieces_suite;

            if (absolute_longest_gap.second < current_axis->longest_gap.second)
                absolute_longest_gap = current_axis->longest_gap;

            if ((sptr_current_axis = current_axis->link.axis2.lock()))
                current_axis = sptr_current_axis.get();
            else
                break;
        }

        sptr_current_axis = root.lock();
        current_axis = sptr_current_axis.get();
        while (current_axis != nullptr)
        {
            current_axis->longest_adjacent_pieces_suite = absolute_longest_adjacent_pieces_suite;
            current_axis->longest_potential_adjacent_pieces_suite = absolute_longest_potential_adjacent_pieces_suite;
            current_axis->longest_gap = absolute_longest_gap;

            if ((sptr_current_axis = current_axis->link.axis2.lock()))
                current_axis = sptr_current_axis.get();
            else
                break;
        }
    }
}

void axis::sync_axis()
{
    sync_extremitys();
    sync_anchors();
    sync_stats();
}

void axis::update(const coordinates coo)
{
    add_piece(coo);
    sync_axis();
}

axis::axis(Board &board_, bool player_, direction d_, std::pair<coordinates, coordinates> ext)
    : board(board_), player(player_), d(d_), extremity(ext), adj_pcs(board_, d_) {}

// Link
Link::Link(std::weak_ptr<axis> a1, std::weak_ptr<axis> a2) : axis1(a1), axis2(a2) {}

Link &Link::operator=(const Link &other)
{
    axis1 = other.axis1;
    axis2 = other.axis2;
    return *this;
}

// piece
piece::piece(bool exist_, bool player_, coordinates coord_)
    : exist(exist_), player(player_), coord(coord_) {}
