#include "aoc.hpp"
#include "my_std.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

using std::operator""s;
using aoc::skip;

struct Item {
  enum : unsigned char {
    Microchip,
    Generator,
  } type{};
  std::string element;
};

using Items = std::vector<Item>;

struct FloorItems {
  int floor{};
  Items items;
};

const std::unordered_map floor_numbers{
    std::pair{"first"s, 1},
    std::pair{"second"s, 2},
    std::pair{"third"s, 3},
    std::pair{"fourth"s, 4},
};

auto encode_element_names(const auto& floor_items) {
  std::unordered_map<std::string, std::size_t> encoding;
  // TODO(views::join)
  {
    std::size_t id{};
    for (const FloorItems& fi : floor_items) {
      for (const Item& item : fi.items) {
        if (not encoding.contains(item.element)) {
          encoding[item.element] = id++;
        }
      }
    }
  }
  return encoding;
}

// Assumptions:
//   - exactly 4 floors
//   - at most 7 unique elements
//   - 1 elevator
// Encoding requires 4 + 4 * 7 * 2 bits, arranged as follows:
//     bit: encoding
//   [0-4): elevator on floor 1-4
//   [4-8): generator 1 on floor 1-4
//  [8-12): chip 1 on floor 1-4
// [12-16): generator 2 on floor 1-4
// ...
// [56-60): chip 7 on floor 1-4

static constexpr auto N_FLOORS{4UZ};
static constexpr auto N_ELEMENTS{7UZ};
using FloorState = std::bitset<N_FLOORS * (1 + N_ELEMENTS * 2)>;

constexpr decltype(auto) floor_indexes() {
  return views::iota(0UZ, N_FLOORS);
}

constexpr decltype(auto) item_indexes(const FloorState& fs, const auto floor = 0) {
  return views::iota(1UZ, fs.count())
         | views::transform([=](const auto& i) { return i * N_FLOORS + floor; });
}

constexpr decltype(auto) generator_indexes(auto&&... args) {
  return item_indexes(args...) | my_std::views::stride(2);
}

constexpr decltype(auto) microchip_indexes(auto&&... args) {
  return item_indexes(args...) | views::drop(1) | my_std::views::stride(2);
}

constexpr decltype(auto) powering_pair_indexes(auto&&... args) {
  return views::zip(generator_indexes(args...), microchip_indexes(args...));
}

constexpr std::size_t elevator_floor(const FloorState& fs) {
  return std::countr_zero(fs.to_ulong());
}

// Hashing FloorState:
// Each floor contains a number of items belonging to one of 3 groups:
//   1. generators
//   2. powered chips
//   3. unpowered chips
// The specific elements of these items are irrelevant for uniqueness.
template <>
struct std::hash<FloorState> {
  std::size_t operator()(const FloorState& fs) const noexcept {
    enum Type : unsigned char {
      generator = 0,
      unpowered_chip = 1,
      powered_chip = 2,
      N = 3,
    };
    std::array<std::array<std::size_t, Type::N>, N_FLOORS> item_counts{};
    for (auto&& f : floor_indexes()) {
      for (auto&& [g, c] : powering_pair_indexes(fs, f)) {
        item_counts.at(f).at(Type::generator) += int{fs[g]};
        item_counts.at(f).at(Type::unpowered_chip) += int{not fs[g] and fs[c]};
        item_counts.at(f).at(Type::powered_chip) += int{fs[g] and fs[c]};
      }
    }
    std::size_t h{elevator_floor(fs)};
    for (auto&& f : floor_indexes()) {
      for (std::size_t i{}; i < Type::N; ++i) {
        h = (h << 3) | item_counts.at(f).at(i);
      }
    }
    return std::hash<std::size_t>{}(h);
  }
};

bool is_frying_chips(const FloorState& fs) {
  for (auto&& f : floor_indexes()) {
    int generators{};
    int unpowered_chips{};
    for (auto&& [g, c] : powering_pair_indexes(fs, f)) {
      generators += int{fs[g]};
      unpowered_chips += int{not fs[g] and fs[c]};
      if (std::min(generators, unpowered_chips) > 0) {
        return true;
      }
    }
  }
  return false;
}

FloorState move(const int step, FloorState fs, auto&&... bits) {
  (std::invoke(
       [&fs, &step](const auto& bit) {
         fs.set(bit, false);
         fs.set(bit + step, true);
       },
       bits
   ),
   ...);
  return fs;
}
FloorState move_up(auto&&... args) {
  return move(1, args...);
}
FloorState move_down(auto&&... args) {
  return move(-1, args...);
}

class AStar {
 public:
  explicit AStar(const FloorState& start, const FloorState& end)
      : m_end_state{end},
        m_q{{start}},
        m_frontier{{start}},
        m_g_score{{start, 0}},
        m_f_score{{start, AStar::heuristic(start)}} {
  }

  [[nodiscard]]
  static int heuristic(const FloorState& fs) {
    constexpr int low_floor_penalty{2};
    constexpr int generator_factor{4};
    int total_cost{};
    for (auto&& f : floor_indexes()) {
      const int floor_cost{low_floor_penalty * static_cast<int>(N_FLOORS - (f + 1))};
      for (auto&& g : generator_indexes(fs, f)) {
        if (fs[g]) {
          total_cost += generator_factor * floor_cost;
        }
      }
      for (auto&& c : microchip_indexes(fs, f)) {
        if (fs[c]) {
          total_cost += floor_cost;
        }
      }
    }
    return total_cost;
  }

  [[nodiscard]]
  bool is_end_state(const FloorState& fs) const noexcept {
    return fs == m_end_state;
  }

  [[nodiscard]]
  auto reconstruct_path(const FloorState& end) const {
    std::vector<FloorState> path;
    for (auto it{m_parents.find(end)}; it != m_parents.end(); it = m_parents.find(it->second)) {
      path.push_back(it->second);
    }
    return path | views::reverse | ranges::to<std::vector>();
  }

  [[nodiscard]]
  explicit operator bool() const noexcept {
    return not m_frontier.empty();
  }
  [[nodiscard]]
  int f_score(const FloorState& fs) const {
    return score_or_max(m_f_score, fs);
  }
  [[nodiscard]]
  int g_score(const FloorState& fs) const {
    return score_or_max(m_g_score, fs);
  }

  void set_f_score(auto&&... args) {
    set_score(m_f_score, args...);
  }
  void set_g_score(auto&&... args) {
    set_score(m_g_score, args...);
  }

  void set_edge(const FloorState& lhs, const FloorState& rhs) {
    m_parents[lhs] = rhs;
  }

  FloorState pop_state() {
    const FloorState fs{pop_min_f_score_heap()};
    m_frontier.erase(fs);
    return fs;
  }

  auto push_state(const FloorState& fs) {
    if (auto&& [_, is_unique] = m_frontier.insert(fs); is_unique) {
      push_min_f_score_heap(fs);
    }
  }

 private:
  [[nodiscard]]
  int score_or_max(const auto& map, const FloorState& fs) const {
    return map.contains(fs) ? map.at(fs) : std::numeric_limits<int>::max() - 1;
  }

  void set_score(auto& map, const auto& fs, const auto& score) {
    map[fs] = score;
  }

  FloorState pop_min_f_score_heap() {
    ranges::pop_heap(m_q, ranges::greater{}, [this](const auto& fs) { return f_score(fs); });
    FloorState fs{m_q.back()};
    m_q.pop_back();
    return fs;
  }

  void push_min_f_score_heap(const FloorState& fs) {
    m_q.push_back(fs);
    ranges::push_heap(m_q, ranges::greater{}, [this](const auto& fs) { return f_score(fs); });
  }

  FloorState m_end_state;
  std::vector<FloorState> m_q;
  std::unordered_set<FloorState> m_frontier;
  std::unordered_map<FloorState, int> m_g_score;
  std::unordered_map<FloorState, int> m_f_score;
  std::unordered_map<FloorState, FloorState> m_parents;
};

bool a_star_step(auto& a_star) {
  const auto state{a_star.pop_state()};

  if (a_star.is_end_state(state)) {
    return true;
  }
  if (is_frying_chips(state)) {
    return false;
  }

  std::vector<FloorState> adjacent_states;

  const auto elevator{elevator_floor(state)};
  const bool can_go_up{elevator < N_FLOORS - 1};
  const bool can_go_down{elevator > 0};
  for (auto&& i : item_indexes(state, elevator)) {
    if (not state[i]) {
      continue;
    }
    if (can_go_up) {
      adjacent_states.push_back(move_up(state, elevator, i));
    }
    if (can_go_down) {
      adjacent_states.push_back(move_down(state, elevator, i));
    }
    for (auto&& j : item_indexes(state, elevator)) {
      if (j < i or not state[j]) {
        continue;
      }
      if (can_go_up) {
        adjacent_states.push_back(move_up(state, elevator, i, j));
      }
      if (can_go_down) {
        adjacent_states.push_back(move_down(state, elevator, i, j));
      }
    }
  }

  for (FloorState adj : adjacent_states) {
    if (auto&& g_score{a_star.g_score(state) + 1}; g_score < a_star.g_score(adj)) {
      a_star.set_edge(adj, state);
      a_star.set_g_score(adj, g_score);
      a_star.set_f_score(adj, g_score + AStar::heuristic(adj));
      a_star.push_state(adj);
    }
  }

  return false;
}

auto a_star_search(FloorState begin, FloorState end) {
  for (AStar a_star(begin, end); a_star;) {
    if (auto&& done{a_star_step(a_star)}; done) {
      const auto solution{a_star.reconstruct_path(end)};
      return solution.size();
    }
  }
  return std::numeric_limits<std::size_t>::max();
}

FloorState as_end_state(FloorState begin) {
  constexpr auto top_floor{N_FLOORS - 1};
  FloorState end;
  // elevator
  end[top_floor] = true;
  for (auto&& i : item_indexes(begin, top_floor)) {
    end[i] = true;
  }
  return end;
}

std::istream& operator>>(std::istream& is, Item& item) {
  if (std::string elem; is >> elem) {
    if (elem.ends_with("-compatible") and is >> std::ws >> skip("microchip"s)) {
      elem = {elem.begin(), ranges::find(elem, '-')};
      item = {Item::Microchip, elem};
      return is;
    }
    if (not elem.empty() and is >> std::ws >> skip("generator"s)) {
      item = {Item::Generator, elem};
      return is;
    }
  }
  throw std::runtime_error("failed parsing Item");
}

std::istream& operator>>(std::istream& is, Items& items) {
  for (std::string s; is >> std::ws >> s and s != "."s;) {
    if (s == "nothing" and is >> std::ws >> skip("relevant"s)) {
    } else if (Item item; ((s == "and" and is >> s and s == "a") or s == "a") and is >> item) {
      items.push_back(item);
    } else {
      throw std::runtime_error(std::format("unknown item '{}'", s));
    }
  }
  return is;
}

std::istream& operator>>(std::istream& is, FloorItems& fi) {
  if (std::string line; std::getline(is, line)) {
    ranges::replace(line, ',', ' ');
    std::istringstream ls{line};
    if (auto [fname, items]{std::pair{""s, Items{}}};
        ls >> skip("The"s) >> std::ws >> fname and floor_numbers.contains(fname)
        and ls >> std::ws >> skip("floor contains"s) >> items) {
      fi = FloorItems{floor_numbers.at(fname), items};
    } else {
      throw std::runtime_error(std::format("invalid line '{}'", line));
    }
  }
  return is;
}

FloorState parse_init_state(std::string_view path) {
  std::istringstream is{aoc::slurp_file(path)};
  const auto all_floor_items{views::istream<FloorItems>(is) | ranges::to<std::vector>()};
  const auto element_ids{encode_element_names(all_floor_items)};
  FloorState fs;
  // elevator
  fs[0] = true;
  for (const auto& floor_items : all_floor_items) {
    for (const auto& item : floor_items.items) {
      const auto item_id{element_ids.find(item.element)->second};
      const auto item_bit{N_FLOORS * (1 + 2 * item_id + int{item.type == Item::Microchip})};
      const auto floor_bit{floor_items.floor - 1};
      const auto bit{item_bit + floor_bit};
      if (bit >= fs.size()) {
        throw std::runtime_error("too many unique input elements");
      }
      fs[bit] = true;
    }
  }
  return fs;
}

int main() {
  const FloorState init_state{parse_init_state("/dev/stdin")};

  FloorState begin1{init_state};
  const auto part1{a_star_search(begin1, as_end_state(begin1))};

  FloorState begin2{init_state};
  for (std::size_t i{N_FLOORS * init_state.count()}; i < begin2.size(); i += N_FLOORS) {
    begin2[i] = true;
  }
  const auto part2{a_star_search(begin2, as_end_state(begin2))};

  std::println("{} {}", part1, part2);

  return 0;
}
