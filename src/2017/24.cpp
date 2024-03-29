import std;
import aoc;
import my_std;

namespace ranges = std::ranges;
namespace views = std::views;

struct Component {
  int a{}, b{};
  int strength() const {
    return a + b;
  }
};

std::istream& operator>>(std::istream& is, Component& comp) {
  using aoc::skip;
  using std::operator""s;
  if (int a, b; is >> a >> skip("/"s) >> b) {
    comp = {a, b};
  }
  if (is or is.eof()) {
    return is;
  }
  throw std::runtime_error("failed parsing Component");
}

auto find_max_bridges(auto components) {
  constexpr auto state_size{54};
  if (components.size() > state_size) {
    throw std::runtime_error("too many components, cannot use std::bitset");
  }

  int max_part1{}, max_part2{}, max_len{};

  for (std::deque q{std::tuple{0, 0, std::bitset<state_size>{}}}; not q.empty(); q.pop_front()) {
    const auto [port, strength, used] = q.front();

    max_part1 = std::max(max_part1, strength);

    if (const auto n{used.count()}; n > max_len) {
      max_len = n;
      max_part2 = strength;
    } else if (n == max_len) {
      max_part2 = std::max(max_part2, strength);
    }

    for (const auto [i, comp] : my_std::views::enumerate(components)) {
      if (not used[i] and (port == comp.a or port == comp.b)) {
        auto used_next{used};
        used_next[i] = true;
        if (port == comp.a) {
          q.emplace_back(comp.b, strength + comp.strength(), used_next);
        }
        if (port == comp.b) {
          q.emplace_back(comp.a, strength + comp.strength(), used_next);
        }
      }
    }
  }

  return std::pair{max_part1, max_part2};
}

int main() {
  std::istringstream input{aoc::slurp_file("/dev/stdin")};
  const auto components{views::istream<Component>(input) | ranges::to<std::vector>()};

  const auto [part1, part2] = find_max_bridges(components);
  std::print("{} {}\n", part1, part2);

  return 0;
}
