#include "aoc.hpp"
#include "my_std.hpp"
#include "ndvec.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

using aoc::skip;
using std::operator""s;
using Vec2 = ndvec::vec2<int>;

enum struct Direction : char {
  left = '<',
  right = '>',
};

std::istream& operator>>(std::istream& is, Direction& dir) {
  if (char ch{}; is >> ch) {
    switch (ch) {
      case std::to_underlying(Direction::left):
      case std::to_underlying(Direction::right): {
        dir = {ch};
        break;
      }
      default: {
        throw std::runtime_error(std::format("invalid direction '{}'", ch));
      }
    }
  }
  return is;
}

enum struct RockType : unsigned char {
  horizontal,
  plus,
  angle,
  vertical,
  square,
};

struct Rock {
  /* rocks with a shape encoding as a 4x4 boolean grid in row-major order
     ####
     ....
     ....
     ....

     .#..
     ###.
     .#..
     ....

     ..#.
     ..#.
     ###.
     ....

     #...
     #...
     #...
     #...

     ##..
     ##..
     ....
     ....
   */
  Vec2 pos;
  std::bitset<16UZ> shape;
  int width{};
  int height{};

  explicit Rock(Vec2 begin, RockType type) {
    std::vector<int> on;
    switch (type) {
      case RockType::horizontal: {
        on = {0, 1, 2, 3};
        width = 4;
        height = 1;
      } break;
      case RockType::plus: {
        on = {1, 4, 5, 6, 9};
        width = 3;
        height = 3;
      } break;
      case RockType::angle: {
        on = {2, 6, 8, 9, 10};
        width = 3;
        height = 3;
      } break;
      case RockType::vertical: {
        on = {0, 4, 8, 12};
        width = 1;
        height = 4;
      } break;
      case RockType::square: {
        on = {0, 1, 4, 5};
        width = 2;
        height = 2;
      } break;
    }
    pos = begin + Vec2(0, height);
    for (int i : on) {
      shape[i] = true;
    }
  }

  [[nodiscard]]
  auto points() const {
    return (
        views::iota(0UZ, shape.size()) | views::filter([this](auto i) { return shape[i]; })
        | views::transform([this](auto i) {
            auto [y, x]{std::div(i, 4)};
            return pos + Vec2(x, -y);
          })
    );
  }
};

struct Grid {
  static constexpr int width{7};
  int height{};

  std::unordered_set<Vec2> rocks;

  [[nodiscard]]
  bool is_blocked(const Vec2 p) const {
    return p.min() < 0 or p.x() >= width or rocks.contains(p);
  }

  bool move_rock(Rock& r, const Vec2 step) const {
    if (ranges::any_of(r.points(), [this, &step](const Vec2& p) { return is_blocked(p + step); })) {
      return false;
    }
    r.pos += step;
    return true;
  }

  void freeze_rock(const Rock& r) {
    for (Vec2 p : r.points()) {
      rocks.insert(p);
      height = std::max(height, p.y());
    }
  }
};

struct Cycle {
  std::size_t begin{};
  std::size_t length{};
};

std::optional<Cycle> find_cycle(const auto& v) {
  // https://en.wikipedia.org/wiki/Cycle_detection#Tortoise_and_hare
  // accessed 2024-05-12
  if (v.size() < 3) {
    return std::nullopt;
  }

  std::size_t t{1};
  std::size_t h{2};

  while (v.at(t) != v.at(h)) {
    t += 1;
    h += 2;
    if (h >= v.size()) {
      return std::nullopt;
    }
  }

  Cycle c;
  t = 0;

  while (v.at(t) != v.at(h)) {
    t += 1;
    h += 1;
    c.begin += 1;
    if (h >= v.size()) {
      return std::nullopt;
    }
  }

  c.length = 1;
  h = t + 1;

  while (v.at(t) != v.at(h)) {
    h += 1;
    c.length += 1;
  }

  return c;
}

auto simulate_until_cycle(const auto jet) {
  std::vector<RockType> rock_types{
      RockType::horizontal,
      RockType::plus,
      RockType::angle,
      RockType::vertical,
      RockType::square,
  };

  Grid grid;
  std::optional<Rock> rock;
  std::optional<Cycle> cycle;
  std::vector<std::tuple<std::size_t, std::size_t, RockType>> height_deltas;

  for (std::size_t i_rock{}, i_jet{}; not cycle; i_jet = (i_jet + 1) % jet.size()) {
    if (not rock) {
      rock = Rock(Vec2(2, grid.height + 3), rock_types[i_rock]);
      i_rock = (i_rock + 1) % rock_types.size();
    }
    switch (jet[i_jet]) {
      case Direction::left: {
        grid.move_rock(*rock, Vec2(-1, 0));
      } break;
      case Direction::right: {
        grid.move_rock(*rock, Vec2(1, 0));
      } break;
    }
    if (bool did_move{grid.move_rock(*rock, Vec2(0, -1))}; not did_move) {
      const auto height0{grid.height};
      grid.freeze_rock(*rock);
      rock.reset();
      const auto height1{grid.height};
      height_deltas.emplace_back(height1 - height0, i_jet, rock_types[i_rock]);
      if (height_deltas.size() > jet.size()) {
        cycle = find_cycle(height_deltas);
      }
    }
  }

  return std::pair{height_deltas, cycle.value()};
}

auto infer_tower_height(const auto& height_deltas, const auto& cycle, const std::size_t limit) {
  std::size_t tower_height{};
  std::size_t rocks_stopped{};

  for (; rocks_stopped < cycle.begin; rocks_stopped += 1) {
    const auto height{std::get<0>(height_deltas.at(rocks_stopped))};
    tower_height += height;
  }

  std::size_t cycle_delta{};
  std::vector<std::size_t> cycle_deltas(cycle.length);
  for (auto [i, delta] : my_std::views::enumerate(cycle_deltas)) {
    const auto height{std::get<0>(height_deltas.at(i + cycle.begin))};
    delta = height;
    cycle_delta += height;
  }

  for (; rocks_stopped + cycle.length < limit; rocks_stopped += cycle.length) {
    tower_height += cycle_delta;
  }

  for (; rocks_stopped < limit; rocks_stopped += 1) {
    tower_height += cycle_deltas.at((rocks_stopped - cycle.begin) % cycle.length);
  }

  return tower_height;
}

int main() {
  const auto jet{aoc::parse_items<Direction>("/dev/stdin")};

  const auto [height_deltas, cycle]{simulate_until_cycle(jet)};
  const auto part1{infer_tower_height(height_deltas, cycle, 2022)};
  const auto part2{infer_tower_height(height_deltas, cycle, 1'000'000'000'000)};

  std::println("{} {}", part1, part2);

  return 0;
}
