#include "aoc.hpp"
#include "my_std.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

using std::operator""s;

enum class Slot : char {
  amber = 'A',
  bronze = 'B',
  copper = 'C',
  desert = 'D',
  empty = 'E',
};

struct Burrow {
  // encoding:
  //
  // hallway with 11 slots
  // 2-4 rooms with 4 slots each
  // => 27 slots in total
  //
  // slot can be one of:
  //   1. A
  //   2. B
  //   3. C
  //   4. D
  //   5. empty
  // => 5 states per slot

  // => 27 * 5 = 135 states in total
  //
  static constexpr int n_hallway_slots{11};
  static constexpr int n_shrimps{4};
  static constexpr int n_rooms{4};
  static constexpr auto n_states{(n_hallway_slots + n_shrimps * n_rooms) * (n_shrimps + 1)};

  using State = std::bitset<n_states>;
  using Energy = int;
  using Position = int;
  using Move = std::pair<Burrow, Energy>;
  using Moves = std::vector<Move>;

  State state;
  int n_slots{};

  Burrow() = default;

  [[nodiscard]]
  static Energy energy_per_step(Slot s) {
    switch (s) {
      case Slot::amber:
        return 1;
      case Slot::bronze:
        return 10;
      case Slot::copper:
        return 100;
      case Slot::desert:
        return 1000;
      case Slot::empty:
        throw std::runtime_error("energy of empty slot does not make sense");
    }
  }

  [[nodiscard]]
  static Position encode(Slot s) {
    return std::to_underlying(s) - std::to_underlying(Slot::amber);
  }

  [[nodiscard]]
  static Position encode(Position p, Slot s) {
    return (n_shrimps + 1) * p + encode(s);
  }

  [[nodiscard]]
  static bool is_hallway(Position p) {
    return 0 <= p and p < n_hallway_slots;
  }

  [[nodiscard]]
  static bool is_above_home_of(Position p, Slot s) {
    return is_hallway(p) and p == 2 * (encode(s) + 1);
  }

  [[nodiscard]]
  static Position hallway_above_room(Position p) {
    return (p % 10) * 2;
  }

  [[nodiscard]]
  bool is_room(Position p) const {
    return n_hallway_slots <= p and p < n_slots;
  }

  [[nodiscard]]
  bool is_inside(Position p) const {
    return is_hallway(p) or is_room(p);
  }

  [[nodiscard]]
  bool contains(Position p, Slot s) const {
    return is_inside(p) and state[encode(p, s)];
  }

  [[nodiscard]]
  Slot at(Position p) const {
    for (Slot s : {Slot::amber, Slot::bronze, Slot::copper, Slot::desert, Slot::empty}) {
      if (contains(p, s)) {
        return s;
      }
    }
    throw std::runtime_error(std::format("unexpected empty state at {}", p));
  }

  [[nodiscard]]
  bool is_vacant(Position p) const {
    return contains(p, Slot::empty);
  }

  [[nodiscard]]
  bool is_home_of(Position p, Slot s) const {
    return s != Slot::empty and is_room(p) and (p - n_hallway_slots) % n_shrimps == encode(s);
  }

  [[nodiscard]]
  bool is_finalized(Position p) const {
    if (not is_inside(p)) {
      return true;
    }
    return is_home_of(p, at(p)) and is_finalized(p + n_shrimps);
  }

  [[nodiscard]]
  std::optional<Move> find_home(Position begin, Slot s) const {
    std::vector<bool> visited(n_slots);

    for (std::deque q{std::pair{begin, 0}}; not q.empty(); q.pop_front()) {
      auto [pos, steps]{q.front()};

      if (not is_inside(pos) or visited.at(pos)) {
        continue;
      }
      visited.at(pos) = true;

      if (pos != begin and not is_vacant(pos)) {
        continue;
      }

      if (is_home_of(pos, s)) {
        if (auto below{pos + n_shrimps}; is_vacant(below)) {
          q.emplace_back(below, steps + 1);
        } else if (is_finalized(below)) {
          return Move{move(begin, pos, s), steps * energy_per_step(s)};
        }
      } else if (is_above_home_of(pos, s)) {
        q.emplace_back(n_hallway_slots + encode(s), steps + 1);
      } else {
        for (Position adj : {pos - 1, pos + 1}) {
          if (is_hallway(adj)) {
            q.emplace_back(adj, steps + 1);
          }
        }
      }
    }

    return std::nullopt;
  }

  [[nodiscard]]
  Moves find_hallway_slots(Position begin, Slot s) const {
    Moves moves;
    {
      std::vector<bool> visited(n_slots);

      for (std::deque q{std::pair{begin, 0}}; not q.empty(); q.pop_front()) {
        auto [pos, steps]{q.front()};

        if (not is_inside(pos) or visited.at(pos)) {
          continue;
        }
        visited.at(pos) = true;

        if (pos != begin and not is_vacant(pos)) {
          continue;
        }

        if (is_room(pos)) {
          if (auto above{pos - n_shrimps}; is_room(above)) {
            q.emplace_back(above, steps + 1);
          } else {
            q.emplace_back(hallway_above_room(pos), steps + 1);
          }
        } else {
          if (not ranges::contains(std::array{2, 4, 6, 8}, pos)) {
            moves.emplace_back(move(begin, pos, s), steps * energy_per_step(s));
          }
          for (Position adj : {pos - 1, pos + 1}) {
            if (is_hallway(adj)) {
              q.emplace_back(adj, steps + 1);
            }
          }
        }
      }
    }
    return moves;
  }

  explicit Burrow(std::vector<Slot> slots) : n_slots{static_cast<int>(slots.size())} {
    for (Position p{}; p < n_slots; ++p) {
      if (auto i{encode(p, slots.at(p))}; i < state.size()) {
        state[i] = true;
      } else {
        throw std::runtime_error(
            std::format("too many slots {}, increase bitset size and recompile", slots.size())
        );
      }
    }
  }

  [[nodiscard]]
  Moves possible_moves(Position p, Slot s) const {
    if (is_hallway(p)) {
      if (auto home{find_home(p, s)}) {
        return {*home};
      }
    } else if (not is_finalized(p)) {
      return find_hallway_slots(p, s);
    }
    return {};
  }

  [[nodiscard]]
  Moves possible_moves() const {
    return my_std::views::cartesian_product(
               views::iota(Position{}, n_slots),
               std::array{Slot::amber, Slot::bronze, Slot::copper, Slot::desert}
           )
           | views::transform(my_std::apply_fn([this](Position p, Slot s) {
               return this->contains(p, s) ? possible_moves(p, s) : Moves{};
             }))
           | views::join | ranges::to<Moves>();
  }

  [[nodiscard]]
  Burrow move(Position src, Position dst, Slot s) const {
    Burrow next{*this};

    next.state.set(encode(src, s), false);
    next.state.set(encode(src, Slot::empty), true);

    next.state.set(encode(dst, Slot::empty), false);
    next.state.set(encode(dst, s), true);

    return next;
  }

  bool operator==(const Burrow& rhs) const {
    return state == rhs.state;
  }
};

template <>
struct std::hash<Burrow> {
  std::size_t operator()(const Burrow& b) const noexcept {
    return std::hash<Burrow::State>{}(b.state);
  }
};

auto search_min_energy(Burrow begin, Burrow end) {
  std::unordered_map<Burrow, Burrow::Energy> total_energy;

  const auto get_energy{[&total_energy](const Burrow& b) {
    if (not total_energy.contains(b)) {
      total_energy[b] = std::numeric_limits<Burrow::Energy>::max();
    }
    return total_energy[b];
  }};

  const auto push_min_energy_heap{[&get_energy](auto& q, const Burrow& b) {
    q.push_back(b);
    ranges::push_heap(q, ranges::greater{}, get_energy);
  }};

  const auto pop_min_energy_heap{[&get_energy](auto& q) {
    ranges::pop_heap(q, ranges::greater{}, get_energy);
    Burrow b{q.back()};
    q.pop_back();
    return b;
  }};

  std::vector q{{begin}};
  total_energy[begin] = 0;

  for (std::unordered_set<Burrow> visited; not q.empty();) {
    Burrow burrow{pop_min_energy_heap(q)};
    if (burrow == end) {
      break;
    }
    if (auto [_, is_new]{visited.insert(burrow)}; is_new) {
      for (auto [next, energy] : burrow.possible_moves()) {
        const auto energy_to_next{aoc::saturating_add(get_energy(burrow), energy)};
        if (energy_to_next < get_energy(next)) {
          total_energy[next] = energy_to_next;
          push_min_energy_heap(q, next);
        }
      }
    }
  }

  return get_energy(end);
}

std::istream& operator>>(std::istream& is, Slot& s) {
  if (std::underlying_type_t<Slot> ch{}; is >> ch) {
    switch (ch) {
      case std::to_underlying(Slot::empty):
      case std::to_underlying(Slot::amber):
      case std::to_underlying(Slot::bronze):
      case std::to_underlying(Slot::copper):
      case std::to_underlying(Slot::desert): {
        s = {ch};
      } break;
      default: {
        throw std::runtime_error(std::format("unknown slot '{}'", ch));
      }
    }
  }
  return is;
}

Burrow parse_burrow(std::vector<std::string> lines) {
  for (auto& line : lines) {
    ranges::replace(line, '#', ' ');
    ranges::replace(line, '.', std::to_underlying(Slot::empty));
  }

  if (lines.size() != 3 + 2 and lines.size() != 3 + 4) {
    throw std::runtime_error("a burrow should contain 3 lines plus 2 or 4 levels of rooms");
  }

  if (lines[0] != std::string(13, ' ') or not lines.back().starts_with(std::string(11, ' '))) {
    throw std::runtime_error("unexpected burrow structure");
  }

  std::vector<Slot> slots;
  for (std::string line : lines | views::take(lines.size() - 1) | views::drop(1)) {
    std::istringstream ls{line};
    slots.append_range(views::istream<Slot>(ls));
    if (not(ls >> std::ws).eof()) {
      throw std::runtime_error(std::format("failed parsing room '{}'", line));
    }
  }

  if (slots.size() != Burrow::n_hallway_slots + Burrow::n_rooms * (lines.size() - 3)) {
    throw std::runtime_error("missing slots");
  }

  return Burrow(slots);
}

auto find_part1(const auto& lines) {
  Burrow begin{parse_burrow(lines)};
  Burrow end{parse_burrow(
      std::vector{{
          "#############"s,
          "#...........#"s,
          "###A#B#C#D###"s,
          "  #A#B#C#D#"s,
          "  #########"s,
      }}
  )};
  return search_min_energy(begin, end);
}

auto find_part2(auto lines) {
  lines.insert(lines.begin() + 3, "  #D#B#A#C#"s);
  lines.insert(lines.begin() + 3, "  #D#C#B#A#"s);
  Burrow begin{parse_burrow(lines)};
  Burrow end{parse_burrow(
      std::vector{{
          "#############"s,
          "#...........#"s,
          "###A#B#C#D###"s,
          "  #A#B#C#D#"s,
          "  #A#B#C#D#"s,
          "  #A#B#C#D#"s,
          "  #########"s,
      }}
  )};
  return search_min_energy(begin, end);
}

int main() {
  const auto lines{aoc::slurp_lines("/dev/stdin")};

  const auto part1{find_part1(lines)};
  const auto part2{find_part2(lines)};

  std::println("{} {}", part1, part2);

  return 0;
}
