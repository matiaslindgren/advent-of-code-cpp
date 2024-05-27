#include "aoc.hpp"
#include "my_std.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

using aoc::skip;
using std::operator""s;

struct Component {
  int a{};
  int b{};
};

auto find_max_bridges(auto components) {
  constexpr auto state_size{54};
  if (components.size() > state_size) {
    throw std::runtime_error("too many components, cannot use std::bitset");
  }

  int max_part1{};
  int max_part2{};
  std::size_t max_len{};

  for (std::deque q{std::tuple{0, 0, std::bitset<state_size>{}}}; not q.empty(); q.pop_front()) {
    auto [port, strength, used]{q.front()};

    max_part1 = std::max(max_part1, strength);

    if (const auto n{used.count()}; n > max_len) {
      max_len = n;
      max_part2 = strength;
    } else if (n == max_len) {
      max_part2 = std::max(max_part2, strength);
    }

    for (auto&& [i, comp] : my_std::views::enumerate(components)) {
      if (not used[i] and (port == comp.a or port == comp.b)) {
        auto used_next{used};
        used_next[i] = true;
        if (port == comp.a) {
          q.emplace_back(comp.b, strength + comp.a + comp.b, used_next);
        }
        if (port == comp.b) {
          q.emplace_back(comp.a, strength + comp.a + comp.b, used_next);
        }
      }
    }
  }

  return std::pair{max_part1, max_part2};
}

std::istream& operator>>(std::istream& is, Component& comp) {
  if (int a{}, b{}; is >> a >> skip("/"s) >> b) {
    comp = {a, b};
  }
  return is;
}

int main() {
  const auto components{aoc::parse_items<Component>("/dev/stdin")};
  const auto [part1, part2]{find_max_bridges(components)};
  std::println("{} {}", part1, part2);
  return 0;
}
