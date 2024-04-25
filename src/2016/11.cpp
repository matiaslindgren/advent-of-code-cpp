#include "aoc.hpp"
#include "my_std.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

struct Item {
  enum {
    Microchip,
    Generator,
  } type;
  std::string element;
};

std::istream& operator>>(std::istream& is, Item& item) {
  using std::operator""s;
  using aoc::skip;
  if (std::string elem; is >> elem) {
    if (elem.ends_with("-compatible") and is >> skip(" microchip"s)) {
      elem = {elem.begin(), ranges::find(elem, '-')};
      item = {Item::Microchip, elem};
      return is;
    }
    if (not elem.empty() and is >> skip(" generator"s)) {
      item = {Item::Generator, elem};
      return is;
    }
  }
  if (is.eof()) {
    return is;
  }
  throw std::runtime_error("failed parsing Item");
}

struct FloorItems {
  int floor;
  std::vector<Item> items;
};

std::istream& operator>>(std::istream& is, FloorItems& fi) {
  using std::operator""s;
  using aoc::skip;
  static const std::unordered_map<std::string, int> floor_numbers{
      {"first", 1},
      {"second", 2},
      {"third", 3},
      {"fourth", 4},
  };
  int floor;
  std::vector<Item> items;
  if (std::string line; std::getline(is, line)) {
    std::stringstream ls{line};
    if (std::string fname; ls >> skip("The"s) >> std::ws >> fname and floor_numbers.contains(fname)
                           and ls >> std::ws >> skip("floor contains"s)) {
      floor = floor_numbers.find(fname)->second;
      while (ls) {
        if (std::string s; ls >> s) {
          if (s == "nothing" and ls >> std::ws >> skip("relevant"s)) {
          } else if (Item item;
                     ((s == "and" and ls >> s and s == "a") or s == "a") and ls >> item) {
            items.push_back(item);
          } else {
            is.setstate(std::ios_base::failbit);
            ls.setstate(std::ios_base::failbit);
          }
        }
        if (ls) {
          if (auto ch{ls.peek()}; ch == ',' or ch == '.') {
            ls.get();
          }
        }
      }
    }
    if (ls.eof() and is) {
      fi = {floor, items};
      return is;
    }
  }
  if (is.eof()) {
    return is;
  }
  throw std::runtime_error("failed parsing FloorItems");
}

auto encode_element_names(const auto& floor_items) {
  std::unordered_map<std::string, std::size_t> encoding;
  // TODO(views::join)
  {
    std::size_t id{};
    for (const auto& fi : floor_items) {
      for (const auto& item : fi.items) {
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

static constexpr auto N_FLOORS{4uz};
static constexpr auto N_ELEMENTS{7uz};
using FloorState = std::bitset<N_FLOORS * (1 + N_ELEMENTS * 2)>;

constexpr decltype(auto) floor_indexes() {
  return views::iota(0uz, N_FLOORS);
}

constexpr decltype(auto) item_indexes(const FloorState& fs, const auto floor = 0) {
  return views::iota(1uz, fs.count())
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
    enum Type {
      generator = 0,
      unpowered_chip = 1,
      powered_chip = 2,
      N = 3,
    };
    std::size_t item_counts[N_FLOORS][Type::N] = {};
    for (const auto f : floor_indexes()) {
      for (const auto [g, c] : powering_pair_indexes(fs, f)) {
        item_counts[f][Type::generator] += fs[g];
        item_counts[f][Type::unpowered_chip] += not fs[g] and fs[c];
        item_counts[f][Type::powered_chip] += fs[g] and fs[c];
      }
    }
    std::size_t h{elevator_floor(fs)};
    for (const auto f : floor_indexes()) {
      for (std::size_t i{}; i < Type::N; ++i) {
        h = (h << 3) | item_counts[f][i];
      }
    }
    return std::hash<std::size_t>{}(h);
  }
};

FloorState parse_init_state(std::istream& is) {
  const auto all_floor_items{views::istream<FloorItems>(is) | ranges::to<std::vector>()};
  const auto element_ids{encode_element_names(all_floor_items)};
  FloorState fs;
  // elevator
  fs[0] = true;
  for (const auto& floor_items : all_floor_items) {
    for (const auto& item : floor_items.items) {
      const auto item_id{element_ids.find(item.element)->second};
      const auto item_bit{N_FLOORS * (1 + 2 * item_id + (item.type == Item::Microchip))};
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

constexpr bool is_frying_chips(const FloorState& fs) {
  for (const auto f : floor_indexes()) {
    int generators{};
    int unpowered_chips{};
    for (const auto [g, c] : powering_pair_indexes(fs, f)) {
      generators += fs[g];
      unpowered_chips += not fs[g] and fs[c];
      if (generators and unpowered_chips) {
        return true;
      }
    }
  }
  return false;
}

constexpr FloorState move(const int step, FloorState fs, auto&&... bits) {
  (std::invoke(
       [&fs, &step](const auto& bit) {
         fs[bit] = false;
         fs[bit + step] = true;
       },
       bits
   ),
   ...);
  return fs;
}
constexpr FloorState move_up(auto&&... args) {
  return move(1, args...);
}
constexpr FloorState move_down(auto&&... args) {
  return move(-1, args...);
}

class AStar {
 public:
  explicit AStar(const FloorState& start, const FloorState& end)
      : m_end_state{end},
        m_q{{start}},
        m_frontier{{start}},
        m_g_score{{start, 0}},
        m_f_score{{start, h(start)}} {
  }

  constexpr bool is_end_state(const FloorState& fs) const noexcept {
    return fs == m_end_state;
  }

  auto reconstruct_path(const FloorState& end) const {
    std::vector<FloorState> path;
    for (auto it{m_parents.find(end)}; it != m_parents.end(); it = m_parents.find(it->second)) {
      path.push_back(it->second);
    }
    return path | views::reverse | ranges::to<std::vector>();
  }

  constexpr explicit operator bool() const noexcept {
    return not m_frontier.empty();
  }

  constexpr int h(const FloorState& fs) const noexcept {
    constexpr int low_floor_penalty{2};
    constexpr int generator_factor{4};
    int total_cost{};
    for (const auto f : floor_indexes()) {
      const int floor_cost{low_floor_penalty * static_cast<int>(N_FLOORS - (f + 1))};
      for (const auto g : generator_indexes(fs, f)) {
        if (fs[g]) {
          total_cost += generator_factor * floor_cost;
        }
      }
      for (const auto c : microchip_indexes(fs, f)) {
        if (fs[c]) {
          total_cost += floor_cost;
        }
      }
    }
    return total_cost;
  }

  int f_score(const FloorState& fs) const {
    return score_or_max(m_f_score, fs);
  }
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
    if (const auto [_, is_unique] = m_frontier.insert(fs); is_unique) {
      push_min_f_score_heap(fs);
    }
  }

 private:
  int score_or_max(const auto& map, const FloorState& fs) const {
    if (const auto it{map.find(fs)}; it != map.end()) {
      return it->second;
    }
    return std::numeric_limits<int>::max() - 1;
  }

  void set_score(auto& map, const auto& fs, const auto& score) {
    map[fs] = score;
  }

  FloorState pop_min_f_score_heap() {
    ranges::pop_heap(m_q, ranges::greater{}, [=, this](const auto& fs) { return f_score(fs); });
    FloorState fs{m_q.back()};
    m_q.pop_back();
    return fs;
  }

  void push_min_f_score_heap(const FloorState& fs) {
    m_q.push_back(fs);
    ranges::push_heap(m_q, ranges::greater{}, [=, this](const auto& fs) { return f_score(fs); });
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
  for (const auto i : item_indexes(state, elevator)) {
    if (not state[i]) {
      continue;
    }
    if (can_go_up) {
      adjacent_states.push_back(move_up(state, elevator, i));
    }
    if (can_go_down) {
      adjacent_states.push_back(move_down(state, elevator, i));
    }
    for (const auto j : item_indexes(state, elevator)) {
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

  for (FloorState adj_state : adjacent_states) {
    if (const auto g_score{a_star.g_score(state) + 1}; g_score < a_star.g_score(adj_state)) {
      a_star.set_edge(adj_state, state);
      a_star.set_g_score(adj_state, g_score);
      a_star.set_f_score(adj_state, g_score + a_star.h(adj_state));
      a_star.push_state(adj_state);
    }
  }

  return false;
}

auto a_star_search(FloorState begin, FloorState end) {
  for (AStar a_star(begin, end); a_star;) {
    if (const auto done{a_star_step(a_star)}; done) {
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
  for (const auto i : item_indexes(begin, top_floor)) {
    end[i] = true;
  }
  return end;
}

int main() {
  std::istringstream input{aoc::slurp_file("/dev/stdin")};

  const FloorState init_state{parse_init_state(input)};

  FloorState begin1{init_state};
  const auto part1{a_star_search(begin1, as_end_state(begin1))};

  FloorState begin2{init_state};
  for (std::size_t i{N_FLOORS * init_state.count()}; i < begin2.size(); i += N_FLOORS) {
    begin2[i] = true;
  }
  const auto part2{a_star_search(begin2, as_end_state(begin2))};

  std::print("{} {}\n", part1, part2);

  return 0;
}
