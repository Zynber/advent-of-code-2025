#include <format>
#include <fstream>
#include <functional>
#include <iostream>
#include <map>
#include <unordered_map>
#include <vector>

#define DEBUG false

enum class Normal : u_int8_t {
  N = 0b0001,
  E = 0b0010,
  S = 0b0100,
  W = 0b1000,
  NE = N | E,
  SE = S | E,
  SW = S | W,
  NW = N | W
};

// Only apply this on verticals/horizontals!
Normal operator|(Normal a, Normal b) {
  return (Normal) (((u_int8_t) a | (u_int8_t) b) & 0b1111);
}

// Only apply this on diagonals!
Normal operator~(Normal n) { return (Normal) (~((u_int8_t) n) & 0b1111); }

#if DEBUG
const std::unordered_map<Normal, const char *> NORMAL_STRING = {
  {Normal::N, "N"},
  {Normal::E, "E"},
  {Normal::S, "S"},
  {Normal::W, "W"},
  {Normal::NE, "NE"},
  {Normal::SE, "SE"},
  {Normal::SW, "SW"},
  {Normal::NW, "NW"}
};
#endif

enum class Concavity : u_int8_t { NONE, CONVEX, CONCAVE };

Concavity operator~(Concavity c) {
  return (Concavity) (~((u_int8_t) c) & 0b11);
}

#if DEBUG
const std::unordered_map<Concavity, const char *> CONCAVITY_STRING = {
  {Concavity::NONE, "NONE"},
  {Concavity::CONVEX, "CONVEX"},
  {Concavity::CONCAVE, "CONCAVE"}
};
#endif

struct Vec2 {
  const static Vec2 ONE;

  int x, y;

  Vec2 abs() const { return {std::abs(x), std::abs(y)}; }

  std::string to_string() const { return std::format("({}, {})", x, y); }

  Vec2 operator+(const Vec2 &other) const { return {x + other.x, y + other.y}; }

  Vec2 operator-(const Vec2 &other) const { return {x - other.x, y - other.y}; }
};

const Vec2 Vec2::ONE = {1, 1};

struct Tile {
  int idx;
  Vec2 pos;
  Normal normal;
  Concavity concavity;
};

Normal get_edge_normal(const Vec2 &a, const Vec2 &b, bool is_ccw) {
  Vec2 d = b - a;
  std::pair<Normal, Normal> n;

  if (!d.x) {
    n = {Normal::W, Normal::E}; // Downward

    if (b.y < a.y) std::swap(n.first, n.second); // Upward
  } else if (!d.y) {
    n = {Normal::S, Normal::N}; // Rightward

    if (b.x < a.x) std::swap(n.first, n.second); // Leftward
  } else
    throw;

  return is_ccw ? n.first : n.second;
}

// Yeah it is like O(n^3) but honestly I am just happy it works
int main() {
  std::fstream file("input.txt");
  std::string line;
  std::vector<Tile *> tiles;
  std::map<int, std::vector<std::pair<Tile *, Tile *>>> edges_x, edges_y;
  Tile *max_y_tile = nullptr;

  while (std::getline(file, line)) {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
    Tile *tile = new Tile{(int) tiles.size()};
#pragma GCC diagnostic pop
    sscanf(line.c_str(), "%d,%d", &tile->pos.x, &tile->pos.y);

    if (tiles.size()) {
      const auto &[map, pos] = tiles.back()->pos.x == tile->pos.x
                                 ? std::tie(edges_y, tile->pos.x)
                                 : std::tie(edges_x, tile->pos.y);
      auto [it, _] = map.try_emplace(pos);
      it->second.emplace_back(tiles.back(), tile);
    }

    tiles.emplace_back(tile);

    if (!max_y_tile || max_y_tile->pos.y < tile->pos.y ||
        (max_y_tile->pos.y == tile->pos.y && max_y_tile->pos.x > tile->pos.x))
      max_y_tile = tile;
  }

  // CCW => interior is left of edge, CW => interior is right of edge
  const bool is_ccw = [&]() {
    // Examine turn at max-y (lowest) vertex to find polygon orientation
    const Tile *prev =
                 tiles[(max_y_tile->idx + tiles.size() - 1) % tiles.size()],
               *next = tiles[(max_y_tile->idx + 1) % tiles.size()];

    return (next->pos.x - max_y_tile->pos.x) *
               (prev->pos.y - max_y_tile->pos.y) -
             (next->pos.y - max_y_tile->pos.y) *
               (prev->pos.x - max_y_tile->pos.x) <
           0;
  }();

  for (int i = 1; i <= (int) tiles.size(); ++i) {
    Tile *prev = tiles[i - 1], *curr = tiles[i % tiles.size()],
         *next = tiles[(i + 1) % tiles.size()];

    const Normal a = get_edge_normal(prev->pos, curr->pos, is_ccw),
                 b = get_edge_normal(curr->pos, next->pos, is_ccw);

    curr->normal = a | b;
    // What the normal of the first edge needs to be for the corner to be
    // convex, assuming CCW
    Normal to_compare;

    switch (curr->normal) {
      case Normal::N:
      case Normal::E:
      case Normal::S:
      case Normal::W: curr->concavity = Concavity::NONE; continue;
      case Normal::NE: to_compare = Normal::E; break;
      case Normal::SE: to_compare = Normal::S; break;
      case Normal::SW: to_compare = Normal::W; break;
      case Normal::NW: to_compare = Normal::N; break;
    }

    curr->concavity = a == to_compare ? Concavity::CONVEX : Concavity::CONCAVE;

    if (!is_ccw) curr->concavity = ~curr->concavity;

#if DEBUG
    std::cout << prev->pos.to_string() << "-" << curr->pos.to_string() << ":"
              << NORMAL_STRING.at(a) << std::endl;
    std::cout << curr->pos.to_string() << "-" << next->pos.to_string() << ":"
              << NORMAL_STRING.at(b) << std::endl;
    std::cout << curr->pos.to_string() << ": " << NORMAL_STRING.at(curr->normal)
              << ", " << CONCAVITY_STRING.at(curr->concavity) << std::endl;
#endif
  }

  long max_area = 0;
  std::pair<Tile *, Tile *> max_rect;

  for (int i = 0; i < (int) tiles.size() - 1; ++i) {
    for (int j = i + 1; j < (int) tiles.size(); ++j) {
      Tile *a = tiles[i], *b = tiles[j];

      const bool is_plausible = [&]() {
        // How to check whether this is a plausible configuration?
        // Suppose:
        // - Position of corner to check = (x_1, y_1)
        // - Other corner's position     = (x_2, y_2)
        //
        //    XXX                    |     OOO
        // N: O─O => iff y_2 >= y_1  |  S: O─O => iff y_2 <= y_1
        //    OOO                    |     XXX
        //
        //    OOX                    |     xOO
        // E: O│X => iff x_2 <= x_1  |  W: x│O => iff x_2 >= x_1
        //    OOX                    |     XOO
        //
        // =================================================================
        //
        //                    XXX
        // NE convex corner:  O┐X => iff x_2 <= x_1 and y_2 >= y_1
        //                    OOX
        //
        //                    OXX
        // NE concave corner: O└X => iff not (x_2 >= x_1 and y_2 <= y_1)
        //                    OOO    (anti SW convex)
        //
        //                    OOX
        // SE convex corner:  O┘X => iff x_2 <= x_1 and y_2 <= y_1
        //                    XXX
        //
        //                    OOO
        // SE concave corner: O┌X => iff not (x_2 >= x_1 and y_2 >= y_1)
        //                    OXX    (anti NW convex)
        //
        //                    XOO
        // SW convex corner:  X└O => iff x_2 >= x_1 and y_2 <= y_1
        //                    XXX
        //
        //                    OOO
        // SW concave corner: X┐O => iff not (x_2 <= x_1 and y_2 >= y_1)
        //                    XXO    (anti NE convex)
        //
        //                    XXX
        // NW convex corner:  X┌O => iff x_2 >= x_1 and y_2 >= y_1
        //                    XOO
        //
        //                    XXO
        // NW concave corner: X┘O => iff not (x_2 <= x_1 and y_2 <= y_1)
        //                    OOO    (anti SE convex)

        // Predicates to check whether the configuration is plausible
        const std::unordered_map<Normal, std::function<bool(Tile *, Tile *)>>
          predicates = {
            {Normal::N, [](Tile *r, Tile *o) { return o->pos.y >= r->pos.y; }},
            {Normal::S, [](Tile *r, Tile *o) { return o->pos.y <= r->pos.y; }},
            {Normal::E, [](Tile *r, Tile *o) { return o->pos.x <= r->pos.x; }},
            {Normal::W, [](Tile *r, Tile *o) { return o->pos.x >= r->pos.x; }},
            // Concave cases only
            {Normal::NE,
             [](Tile *r, Tile *o) {
               return o->pos.x <= r->pos.x && o->pos.y >= r->pos.y;
             }},
            {Normal::SE,
             [](Tile *r, Tile *o) {
               return o->pos.x <= r->pos.x && o->pos.y <= r->pos.y;
             }},
            {Normal::SW,
             [](Tile *r, Tile *o) {
               return o->pos.x >= r->pos.x && o->pos.y <= r->pos.y;
             }},
            {Normal::NW, [](Tile *r, Tile *o) {
               return o->pos.x >= r->pos.x && o->pos.y >= r->pos.y;
             }},
          };

        for (const auto &[root, other] : {std::tie(a, b), std::tie(b, a)}) {
          const auto predicate = predicates.at(
            root->concavity == Concavity::CONCAVE ? ~root->normal : root->normal
          );

          const bool result = predicate(root, other);

          if (root->concavity == Concavity::CONCAVE ? result : !result)
            return false;
        }

        return true;
      }();

      if (!is_plausible) {
#if DEBUG
        std::cout << "[" << a->pos.to_string() << ", " << b->pos.to_string()
                  << "]: Not plausible" << std::endl;
#endif
        continue;
      }

      Vec2 min, max;
      std::tie(min.x, max.x) = std::minmax(a->pos.x, b->pos.x);
      std::tie(min.y, max.y) = std::minmax(a->pos.y, b->pos.y);

      bool is_obstructed = [&]() {
        // Check whether an edge cuts into/across the candidate rect
        for (auto [primary_comp_ptr, secondary_comp_ptr, edges] :
             {std::tuple(&Vec2::x, &Vec2::y, edges_x),
              std::tuple(&Vec2::y, &Vec2::x, edges_y)}) {
          auto start_it = edges.lower_bound(min.*secondary_comp_ptr),
               end_it = edges.upper_bound(max.*secondary_comp_ptr);

          for (auto it = start_it; it != end_it; ++it) {
            const int comp = it->first;

            if (comp == min.*secondary_comp_ptr ||
                comp == max.*secondary_comp_ptr)
              continue;

            for (const auto &edge : it->second) {
              const auto [edge_min_comp, edge_max_comp] = std::minmax(
                edge.first->pos.*primary_comp_ptr,
                edge.second->pos.*primary_comp_ptr
              );

              // If an edge cuts through the entire rect with both vertices
              // being beyond the rect's boundary
              if (edge_min_comp < min.*primary_comp_ptr &&
                  edge_max_comp > max.*primary_comp_ptr) {
#if DEBUG
                std::cout << "Following rect failed because of edge ["
                          << edge.first->pos.to_string() << ", "
                          << edge.second->pos.to_string() << "]:" << std::endl;
#endif
                return true;
              }

              // If a vertex lands within the rect
              for (int c : {edge_min_comp, edge_max_comp})
                if (min.*primary_comp_ptr < c && c < max.*primary_comp_ptr) {
#if DEBUG
                  std::cout << "Following rect failed because of edge ["
                            << edge.first->pos.to_string() << ", "
                            << edge.second->pos.to_string()
                            << "]:" << std::endl;
#endif
                  return true;
                }
            }
          }
        }

        return false;
      }();

      if (is_obstructed) {
#if DEBUG
        std::cout << "[" << a->pos.to_string() << ", " << b->pos.to_string()
                  << "]: Obstructed" << std::endl;
#endif
        continue;
      }

      Vec2 rect = max - min + Vec2::ONE;
      const long area = 1L * rect.x * rect.y;

      if (area > max_area) {
        max_area = std::max(max_area, area);
        max_rect = {a, b};
      }
#if DEBUG
      std::cout << "[" << a->pos.to_string() << ", " << b->pos.to_string()
                << "]: area = " << area << ", max_area = " << max_area
                << std::endl;
#endif
    }
  }

  std::cout << "[" << max_rect.first->pos.to_string() << ", "
            << max_rect.second->pos.to_string() << "] forms rect with area of "
            << max_area << std::endl;
  return 0;
}
