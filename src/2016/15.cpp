#include "aoc.hpp"
#include "my_std.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

struct Disc {
  int pos;
  int mod;
};

std::istream& operator>>(std::istream& is, Disc& disc) {
  using aoc::skip;
  using std::operator""s;
  if (std::string line; std::getline(is, line)) {
    std::istringstream ls{line};
    if (int id, mod, time, pos;
        ls >> skip("Disc #"s) >> id and id > 0
        and ls >> skip(" has"s) >> mod >> skip(" positions;"s) >> skip(" at time="s) >> time
        and time == 0 and ls >> skip(", it is at position"s) >> pos >> skip("."s)) {
      disc = {pos, mod};
      return is;
    }
  }
  if (is.eof()) {
    return is;
  }
  throw std::runtime_error("failed parsing Disc");
}

auto find_time_to_press(const auto& discs) {
  // TODO no loop, find common multiple/divisor
  for (auto t{0UZ}; t < 100'000'000; ++t) {
    const auto has_open_slot{[=](const auto& p) {
      const auto& [i, disc] = p;
      return ((disc.pos + t + i + 1) % disc.mod) == 0;
    }};
    if (ranges::all_of(my_std::views::enumerate(discs), has_open_slot)) {
      return t;
    }
  }
  return std::numeric_limits<std::size_t>::max();
}

int main() {
  auto discs{aoc::parse_items<Disc>("/dev/stdin")};

  const auto part1{find_time_to_press(discs)};

  discs.push_back({0, 11});
  const auto part2{find_time_to_press(discs)};

  std::println("{} {}", part1, part2);

  return 0;
}
